/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/application_filter.h
 * PURPOSE:
 *   Filter existing application choices without changing their selection or lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_APPLICATION_FILTER_H
#define UMICOM_DESKTOP_APPLICATION_FILTER_H
#include "umicom/application/launch_selection.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDeskApplicationFilter {
    UMI_DESK_APPLICATION_FILTER_ALL = 0,
    UMI_DESK_APPLICATION_FILTER_AVAILABLE = 1,
    UMI_DESK_APPLICATION_FILTER_RUNNING = 2,
    UMI_DESK_APPLICATION_FILTER_SELECTED = 3,
    UMI_DESK_APPLICATION_FILTER_ATTENTION = 4
} UmiDeskApplicationFilter;

/** Query one existing choice. Available uses its current eligible flag;
 * Running and Selected use their corresponding flags. Attention means the
 * explicit ATTENTION or FAILED lifecycle states, not a guessed failed process.
 * Selection is not changed when a row is hidden. Invalid pointers, mode or
 * lifecycle state return INVALID_ARGUMENT with outMatches unchanged.
 * This function borrows only for the call and has no I/O or side effects.
 * Combine its result with a name search in the frontend. See the Desk tests. */
UmiStatus UmiDeskApplicationFilterMatches(const UmiApplicationLaunchChoice *choice,
    UmiDeskApplicationFilter filter, bool *outMatches);
#ifdef __cplusplus
}
#endif
#endif
