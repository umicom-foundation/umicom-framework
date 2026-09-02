/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/layout_lock.h
 *
 * PURPOSE:
 *   Represent layout lock ownership and mutation policy.
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
#ifndef UMICOM_DESKTOP_CONTROL_LAYOUT_LOCK_H
#define UMICOM_DESKTOP_CONTROL_LAYOUT_LOCK_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_LAYOUT_LOCK_API_VERSION 1U

/**
 * Represent the desktop layout lock snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopLayoutLockSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopLayoutLockSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_layout_lock_init(UmiDesktopLayoutLockSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_layout_lock_validate(const UmiDesktopLayoutLockSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_layout_lock_set_subject(UmiDesktopLayoutLockSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_layout_lock_set_detail(UmiDesktopLayoutLockSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_layout_lock_set_state(UmiDesktopLayoutLockSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_layout_lock_set_priority(UmiDesktopLayoutLockSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_layout_lock_set_enabled(UmiDesktopLayoutLockSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_layout_lock_same_identity(const UmiDesktopLayoutLockSnapshot *left, const UmiDesktopLayoutLockSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_layout_lock_mutation_allowed(bool locked, const char *lock_owner, const char *request_owner);

#ifdef __cplusplus
}
#endif
#endif
