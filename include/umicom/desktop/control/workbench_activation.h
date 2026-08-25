/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/workbench_activation.h
 *
 * PURPOSE:
 *   Track active workbench/perspective selection as Framework state.
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
#ifndef UMICOM_DESKTOP_CONTROL_WORKBENCH_ACTIVATION_H
#define UMICOM_DESKTOP_CONTROL_WORKBENCH_ACTIVATION_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_WORKBENCH_ACTIVATION_API_VERSION 1U

typedef struct UmiDesktopWorkbenchActivationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopWorkbenchActivationSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_workbench_activation_init(UmiDesktopWorkbenchActivationSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_workbench_activation_validate(const UmiDesktopWorkbenchActivationSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_workbench_activation_set_subject(UmiDesktopWorkbenchActivationSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_workbench_activation_set_detail(UmiDesktopWorkbenchActivationSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_workbench_activation_set_state(UmiDesktopWorkbenchActivationSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_workbench_activation_set_priority(UmiDesktopWorkbenchActivationSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_workbench_activation_set_enabled(UmiDesktopWorkbenchActivationSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_workbench_activation_same_identity(const UmiDesktopWorkbenchActivationSnapshot *left, const UmiDesktopWorkbenchActivationSnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
