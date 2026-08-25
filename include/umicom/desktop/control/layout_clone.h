/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/layout_clone.h
 *
 * PURPOSE:
 *   Represent a clone operation from one semantic layout identity to another.
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
#ifndef UMICOM_DESKTOP_CONTROL_LAYOUT_CLONE_H
#define UMICOM_DESKTOP_CONTROL_LAYOUT_CLONE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_LAYOUT_CLONE_API_VERSION 1U

typedef struct UmiDesktopLayoutCloneSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopLayoutCloneSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_layout_clone_init(UmiDesktopLayoutCloneSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_layout_clone_validate(const UmiDesktopLayoutCloneSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_layout_clone_set_subject(UmiDesktopLayoutCloneSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_layout_clone_set_detail(UmiDesktopLayoutCloneSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_layout_clone_set_state(UmiDesktopLayoutCloneSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_layout_clone_set_priority(UmiDesktopLayoutCloneSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_layout_clone_set_enabled(UmiDesktopLayoutCloneSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_layout_clone_same_identity(const UmiDesktopLayoutCloneSnapshot *left, const UmiDesktopLayoutCloneSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
