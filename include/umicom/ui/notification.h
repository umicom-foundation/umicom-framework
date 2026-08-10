/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/notification.h
 *
 * PURPOSE:
 *   Define a bounded notification centre with severity, commands, dismissal and
 *   chronological snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiUiNotification {
    uint64_t notification_id;
    UmiUiSeverity severity;
    char title[UMI_UI_TEXT_CAPACITY];
    char message[UMI_UI_DESCRIPTION_CAPACITY];
    char action_id[UMI_UI_ID_CAPACITY];
    uint64_t timestamp_ns;
    int dismissed;
} UmiUiNotification;

typedef struct UmiUiNotificationCentre UmiUiNotificationCentre;
UmiStatus umi_ui_notification_centre_create(UmiUiNotificationCentre **out_centre);
void umi_ui_notification_centre_destroy(UmiUiNotificationCentre *centre);
UmiStatus umi_ui_notification_publish(UmiUiNotificationCentre *centre,
                                      const UmiUiNotification *notification,
                                      uint64_t *out_notification_id);
UmiStatus umi_ui_notification_dismiss(UmiUiNotificationCentre *centre, uint64_t id);
UmiStatus umi_ui_notification_at(const UmiUiNotificationCentre *centre, size_t index,
                                 UmiUiNotification *out_notification);
size_t umi_ui_notification_count(const UmiUiNotificationCentre *centre, int include_dismissed);
void umi_ui_notification_clear(UmiUiNotificationCentre *centre);

#ifdef __cplusplus
}
#endif

#endif
