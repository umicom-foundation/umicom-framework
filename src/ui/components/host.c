/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/host.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/host.h"

#include <string.h>

UmiStatus umi_ui_component_host_init(UmiUiComponentHost *host,
                                     const char *kind,
                                     void *adapter_handle)
{
    if (host == NULL || kind == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const size_t length = strlen(kind);
    if (length >= sizeof(host->kind)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(host->kind, kind, length + 1U);
    host->adapter_handle = adapter_handle;
    return UMI_STATUS_OK;
}
