/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/base/status.c
 *
 * PURPOSE:
 *   Implement the status behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/status.h"

const char *umi_status_text(UmiStatus status)
{
    switch (status) {
        case UMI_STATUS_OK: return "OK";
        case UMI_STATUS_INVALID_ARGUMENT: return "Invalid argument";
        case UMI_STATUS_INVALID_STATE: return "Invalid state";
        case UMI_STATUS_CAPACITY_EXCEEDED: return "Capacity exceeded";
        case UMI_STATUS_NOT_FOUND: return "Not found";
        case UMI_STATUS_ALREADY_EXISTS: return "Already exists";
        case UMI_STATUS_OUT_OF_MEMORY: return "Out of memory";
        case UMI_STATUS_PERMISSION_DENIED: return "Permission denied";
        case UMI_STATUS_IO_ERROR: return "Input/output error";
        case UMI_STATUS_PARSE_ERROR: return "Parse error";
        case UMI_STATUS_UNAVAILABLE: return "Unavailable";
        case UMI_STATUS_NOT_IMPLEMENTED: return "Not implemented";
        case UMI_STATUS_INTERNAL_ERROR: return "Internal error";
        default: return "Unknown status";
    }
}
