/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/control/notification_centre.h
 *
 * PURPOSE:
 *   Represent notification-centre item metadata and read/priority state.
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
#ifndef UMICOM_DESKTOP_CONTROL_NOTIFICATION_CENTRE_H
#define UMICOM_DESKTOP_CONTROL_NOTIFICATION_CENTRE_H

#include "umicom/desktop/control/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_NOTIFICATION_CENTRE_API_VERSION 1U

typedef struct UmiDesktopNotificationCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char subject_id[UMI_DESKTOP_CONTROL_ID_CAPACITY];
    char detail[UMI_DESKTOP_CONTROL_TEXT_CAPACITY];
    uint32_t state;
    uint32_t priority;
    uint64_t revision;
    bool enabled;
} UmiDesktopNotificationCentreSnapshot;

/* Initialise a valid bounded snapshot with stable identity. */
void umi_desktop_notification_centre_init(UmiDesktopNotificationCentreSnapshot *value, const char *id);
/* Validate structure version, identity and bounded string invariants. */
UmiStatus umi_desktop_notification_centre_validate(const UmiDesktopNotificationCentreSnapshot *value);
/* Associate the control record with an application, panel, workbench or layout identity. */
UmiStatus umi_desktop_notification_centre_set_subject(UmiDesktopNotificationCentreSnapshot *value, const char *subject_id);
/* Store human-readable diagnostic/policy detail without silent truncation. */
UmiStatus umi_desktop_notification_centre_set_detail(UmiDesktopNotificationCentreSnapshot *value, const char *detail);
/* Update numeric state and increment the monotonic local revision. */
UmiStatus umi_desktop_notification_centre_set_state(UmiDesktopNotificationCentreSnapshot *value, uint32_t state);
/* Update deterministic ordering/ranking priority. */
UmiStatus umi_desktop_notification_centre_set_priority(UmiDesktopNotificationCentreSnapshot *value, uint32_t priority);
/* Toggle the record while retaining identity for layout/session restoration. */
UmiStatus umi_desktop_notification_centre_set_enabled(UmiDesktopNotificationCentreSnapshot *value, bool enabled);
/* Identity comparison deliberately ignores mutable state. */
bool umi_desktop_notification_centre_same_identity(const UmiDesktopNotificationCentreSnapshot *left, const UmiDesktopNotificationCentreSnapshot *right);

/* Feature-specific policy helper keeps this decision in Framework rather than a thin application. */
bool umi_desktop_notification_centre_visible(UmiDesktopControlSeverity severity, UmiDesktopControlSeverity minimum_severity, bool dismissed);

#ifdef __cplusplus
}
#endif
#endif
