/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/search_session.h
 *
 * PURPOSE:
 *   Represent one global search session and selection state.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_SEARCH_SESSION_H
#define UMICOM_DESKTOP_CONTROL_SEARCH_SESSION_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SEARCH_SESSION_API_VERSION 1U

typedef struct UmiDesktopSearchSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSearchSessionSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_search_session_init(UmiDesktopSearchSessionSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_search_session_validate(const UmiDesktopSearchSessionSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_search_session_set_subject(UmiDesktopSearchSessionSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_search_session_set_detail(UmiDesktopSearchSessionSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_search_session_set_state(UmiDesktopSearchSessionSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_search_session_set_priority(UmiDesktopSearchSessionSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_search_session_set_enabled(UmiDesktopSearchSessionSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_search_session_same_identity(const UmiDesktopSearchSessionSnapshot *left, const UmiDesktopSearchSessionSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
