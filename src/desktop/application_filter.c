/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/application_filter.c
 * PURPOSE:
 *   Keep application filters separate from launch and selection operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/application_filter.h"

UmiStatus UmiDeskApplicationFilterMatches(const UmiApplicationLaunchChoice *choice,
    UmiDeskApplicationFilter filter, bool *outMatches)
{
    if (choice == NULL || outMatches == NULL ||
        filter < UMI_DESK_APPLICATION_FILTER_ALL || filter > UMI_DESK_APPLICATION_FILTER_ATTENTION ||
        choice->state < UMI_APPLICATION_RUNTIME_UNKNOWN ||
        choice->state > UMI_APPLICATION_RUNTIME_STOPPING)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool result = false;
    switch (filter) {
    case UMI_DESK_APPLICATION_FILTER_ALL: result = true; break;
    case UMI_DESK_APPLICATION_FILTER_AVAILABLE: result = choice->eligible; break;
    case UMI_DESK_APPLICATION_FILTER_RUNNING: result = choice->running; break;
    case UMI_DESK_APPLICATION_FILTER_SELECTED: result = choice->selected; break;
    case UMI_DESK_APPLICATION_FILTER_ATTENTION:
        result = choice->state == UMI_APPLICATION_RUNTIME_ATTENTION ||
                 choice->state == UMI_APPLICATION_RUNTIME_FAILED; break;
    }
    *outMatches = result;
    return UMI_STATUS_OK;
}
