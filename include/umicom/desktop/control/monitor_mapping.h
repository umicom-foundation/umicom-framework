/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/monitor_mapping.h
 *
 * PURPOSE:
 *   Describe persisted-to-current monitor mapping decisions.
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
#ifndef UMICOM_DESKTOP_CONTROL_MONITOR_MAPPING_H
#define UMICOM_DESKTOP_CONTROL_MONITOR_MAPPING_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_MONITOR_MAPPING_API_VERSION 1U

typedef struct UmiDesktopMonitorMappingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopMonitorMappingSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_monitor_mapping_init(UmiDesktopMonitorMappingSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_monitor_mapping_validate(const UmiDesktopMonitorMappingSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_monitor_mapping_set_subject(UmiDesktopMonitorMappingSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_monitor_mapping_set_detail(UmiDesktopMonitorMappingSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_monitor_mapping_set_state(UmiDesktopMonitorMappingSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_monitor_mapping_set_priority(UmiDesktopMonitorMappingSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_monitor_mapping_set_enabled(UmiDesktopMonitorMappingSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_monitor_mapping_same_identity(const UmiDesktopMonitorMappingSnapshot *left, const UmiDesktopMonitorMappingSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
uint32_t umi_desktop_monitor_mapping_confidence(bool exact_identity, bool same_geometry, bool primary_fallback);

#ifdef __cplusplus
}
#endif
#endif
