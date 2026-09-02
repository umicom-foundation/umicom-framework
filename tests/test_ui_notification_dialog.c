/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_notification_dialog.c
 *
 * PURPOSE:
 *   Verify notification dismissal and asynchronous dialog completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiNotificationCentre *centre = NULL;
    UmiUiDialogService *dialogs = NULL;
    UmiUiNotification notification = {0};
    UmiUiDialogRequest request = {0};
    UmiUiDialogRequest pending;
    UmiUiDialogResult result = {0};
    uint64_t notification_id = 0U;
    uint64_t request_id = 0U;
    assert(umi_ui_notification_centre_create(&centre) == UMI_STATUS_OK);
    (void)snprintf(notification.title, sizeof(notification.title), "%s", "Build complete");
    notification.severity = UMI_UI_SEVERITY_SUCCESS;
    assert(umi_ui_notification_publish(centre, &notification, &notification_id) == UMI_STATUS_OK);
    assert(umi_ui_notification_dismiss(centre, notification_id) == UMI_STATUS_OK);
    assert(umi_ui_notification_count(centre, 0) == 0U);
    assert(umi_ui_dialog_service_create(&dialogs) == UMI_STATUS_OK);
    request.kind = UMI_UI_DIALOG_CONFIRM;
    (void)snprintf(request.title, sizeof(request.title), "%s", "Confirm");
    (void)snprintf(request.buttons[0], sizeof(request.buttons[0]), "%s", "Yes");
    request.button_count = 1U;
    assert(umi_ui_dialog_request(dialogs, &request, &request_id) == UMI_STATUS_OK);
    assert(umi_ui_dialog_next(dialogs, &pending) == UMI_STATUS_OK);
    result.request_id = pending.request_id; result.accepted = 1;
    assert(umi_ui_dialog_complete(dialogs, &result) == UMI_STATUS_OK);
    assert(umi_ui_dialog_result(dialogs, request_id, &result) == UMI_STATUS_OK);
    assert(result.accepted == 1);
    umi_ui_dialog_service_destroy(dialogs);
    umi_ui_notification_centre_destroy(centre);
    return EXIT_SUCCESS;
}
