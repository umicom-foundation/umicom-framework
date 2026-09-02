/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/search_scope.h
 *
 * PURPOSE:
 *   Describe reusable scope filters for applications, panels, commands and resources.
 *
 * ARCHITECTURE:
 *   This is additive Framework-owned Desk control state. It extends the
 *   established desktop/workbench/layout runtime; it does not replace existing
 *   models and applications remain thin consumers of this public contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTROL_SEARCH_SCOPE_H
#define UMICOM_DESKTOP_CONTROL_SEARCH_SCOPE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_SEARCH_SCOPE_API_VERSION 1U

/**
 * Represent the desktop search scope snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopSearchScopeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopSearchScopeSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_search_scope_init(UmiDesktopSearchScopeSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_search_scope_validate(const UmiDesktopSearchScopeSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_search_scope_set_subject(UmiDesktopSearchScopeSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_search_scope_set_detail(UmiDesktopSearchScopeSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_search_scope_set_state(UmiDesktopSearchScopeSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_search_scope_set_priority(UmiDesktopSearchScopeSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_search_scope_set_enabled(UmiDesktopSearchScopeSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_search_scope_same_identity(const UmiDesktopSearchScopeSnapshot *left, const UmiDesktopSearchScopeSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
