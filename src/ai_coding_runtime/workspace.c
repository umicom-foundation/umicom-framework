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

UmiStatus umi_ai_coding_workspace_adapter_validate(
    const UmiAiCodingWorkspaceAdapter *adapter)
{
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

UmiStatus umi_ai_coding_workspace_file_adapter(
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingFileAdapter *out_adapter)
{
    UmiStatus status;

    if (out_adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_coding_workspace_adapter_validate(workspace);
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
