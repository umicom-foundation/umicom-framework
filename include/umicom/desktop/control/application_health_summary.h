/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/application_health_summary.h
 *
 * PURPOSE:
 *   Aggregate reusable application health state for Desk, Studio and other shells.
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
#ifndef UMICOM_DESKTOP_CONTROL_APPLICATION_HEALTH_SUMMARY_H
#define UMICOM_DESKTOP_CONTROL_APPLICATION_HEALTH_SUMMARY_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_APPLICATION_HEALTH_SUMMARY_API_VERSION 1U

/**
 * Represent the desktop application health summary snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiDesktopApplicationHealthSummarySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopApplicationHealthSummarySnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_application_health_summary_init(UmiDesktopApplicationHealthSummarySnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_application_health_summary_validate(const UmiDesktopApplicationHealthSummarySnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_application_health_summary_set_subject(UmiDesktopApplicationHealthSummarySnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_application_health_summary_set_detail(UmiDesktopApplicationHealthSummarySnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_application_health_summary_set_state(UmiDesktopApplicationHealthSummarySnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_application_health_summary_set_priority(UmiDesktopApplicationHealthSummarySnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_application_health_summary_set_enabled(UmiDesktopApplicationHealthSummarySnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_application_health_summary_same_identity(const UmiDesktopApplicationHealthSummarySnapshot *left, const UmiDesktopApplicationHealthSummarySnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
UmiDesktopControlSeverity umi_desktop_application_health_summary_severity(uint32_t failed_checks, uint32_t degraded_checks);

/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_desktop_application_health_summary_init_checked(UmiDesktopApplicationHealthSummarySnapshot *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
