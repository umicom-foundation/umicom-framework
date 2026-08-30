/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/host_adapter.c
 *
 * PURPOSE:
 *   Validate the thin Studio host callback boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/host_adapter.h"

UmiStatus umi_studio_host_adapter_validate(
    const UmiStudioRuntimeHostAdapter *adapter)
{
    if (adapter == NULL ||
        adapter->set_window_title == NULL ||
        adapter->present_surface == NULL ||
        adapter->update_status == NULL ||
        adapter->update_document_tabs == NULL ||
        adapter->update_workflow == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
