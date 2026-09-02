/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/notification.h
 *
 * PURPOSE:
 *   Define a bounded notification centre with severity, commands, dismissal and
 *   chronological snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_NOTIFICATION_H
#define UMICOM_UI_NOTIFICATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_NOTIFICATION_MAX 256U

/**
 * Represent the ui notification data shared with callers of this public contract.
 */
typedef struct UmiUiNotification {
    uint64_t notification_id;
    UmiUiSeverity severity;
    char title[UMI_UI_TEXT_CAPACITY];
    char message[UMI_UI_DESCRIPTION_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    uint64_t timestamp_ns;
    int dismissed;
} UmiUiNotification;

/**
 * Represent the ui notification centre data shared with callers of this public contract.
 */
typedef struct UmiUiNotificationCentre UmiUiNotificationCentre;
/**
 * Initialise ui notification centre from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_notification_centre_create(UmiUiNotificationCentre **out_centre);
/**
 * Release or reset state held by ui notification centre so the same storage can be reused
 * safely.
 */
void umi_ui_notification_centre_destroy(UmiUiNotificationCentre *centre);
/**
 * Provide the ui notification publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_notification_publish(UmiUiNotificationCentre *centre,
                                      const UmiUiNotification *notification,
                                      uint64_t *out_notification_id);
/**
 * Provide the ui notification dismiss operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_notification_dismiss(UmiUiNotificationCentre *centre, uint64_t id);
/**
 * Find ui notification while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_notification_at(const UmiUiNotificationCentre *centre, size_t index,
                                 UmiUiNotification *out_notification);
/**
 * Return the number of records represented by ui notification without changing their
 * state.
 */
size_t umi_ui_notification_count(const UmiUiNotificationCentre *centre, int include_dismissed);
/**
 * Release or reset state held by ui notification so the same storage can be reused safely.
 */
void umi_ui_notification_clear(UmiUiNotificationCentre *centre);

#ifdef __cplusplus
}
#endif

#endif
