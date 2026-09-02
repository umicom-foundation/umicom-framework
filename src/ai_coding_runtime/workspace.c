/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/workspace.c
 *
 * PURPOSE:
 *   Validate workspace adapters and adapt them to the established governed
 *   UmiAiCodingFileAdapter patch boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/workspace.h"

#include <string.h>

/*
 * Check that ai coding workspace adapter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ai_coding_workspace_adapter_validate(
    const UmiAiCodingWorkspaceAdapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL ||
        adapter->structure_size < sizeof(*adapter) ||
        adapter->api_version != UMI_AI_CODING_RUNTIME_API_VERSION ||
        adapter->read == NULL ||
        adapter->write == NULL ||
        adapter->remove == NULL ||
        adapter->exists == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding workspace file adapter operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_workspace_file_adapter(
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingFileAdapter *out_adapter)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_workspace_adapter_validate(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_adapter, 0, sizeof(*out_adapter));
    out_adapter->structure_size = (uint32_t)sizeof(*out_adapter);
    out_adapter->abi_version = UMI_AI_CODING_ABI_VERSION;
    out_adapter->read = workspace->read;
    out_adapter->write = workspace->write;
    out_adapter->remove = workspace->remove;
    out_adapter->user_data = workspace->user_data;
    return UMI_STATUS_OK;
}
