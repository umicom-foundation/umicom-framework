/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/workbench_registry.h
 *
 * PURPOSE:
 *   Describe registered semantic workbenches without duplicating UI registry internals.
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
#ifndef UMICOM_DESKTOP_CONTROL_WORKBENCH_REGISTRY_H
#define UMICOM_DESKTOP_CONTROL_WORKBENCH_REGISTRY_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_WORKBENCH_REGISTRY_API_VERSION 1U

typedef struct UmiDesktopWorkbenchRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopWorkbenchRegistrySnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_workbench_registry_init(UmiDesktopWorkbenchRegistrySnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_workbench_registry_validate(const UmiDesktopWorkbenchRegistrySnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_workbench_registry_set_subject(UmiDesktopWorkbenchRegistrySnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_workbench_registry_set_detail(UmiDesktopWorkbenchRegistrySnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_workbench_registry_set_state(UmiDesktopWorkbenchRegistrySnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_workbench_registry_set_priority(UmiDesktopWorkbenchRegistrySnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_workbench_registry_set_enabled(UmiDesktopWorkbenchRegistrySnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_workbench_registry_same_identity(const UmiDesktopWorkbenchRegistrySnapshot *left, const UmiDesktopWorkbenchRegistrySnapshot *right);

#ifdef __cplusplus
}
#endif
#endif
