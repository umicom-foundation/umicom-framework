/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/dialog.h
 *
 * PURPOSE:
 *   Define asynchronous, toolkit-neutral dialog requests and results that frontend
 *   adapters can present without leaking native widget handles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_DIALOG_H
#define UMICOM_UI_DIALOG_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"
#include "umicom/ui/value.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_DIALOG_BUTTON_MAX 8U
#define UMI_UI_DIALOG_QUEUE_MAX 64U

typedef enum UmiUiDialogKind { UMI_UI_DIALOG_MESSAGE=1, UMI_UI_DIALOG_CONFIRM=2, UMI_UI_DIALOG_INPUT=3, UMI_UI_DIALOG_FILE=4 } UmiUiDialogKind;
typedef struct UmiUiDialogRequest { uint64_t request_id; UmiUiDialogKind kind; UmiUiSeverity severity; char title[UMI_UI_TEXT_CAPACITY]; char message[UMI_UI_DESCRIPTION_CAPACITY]; char default_value[UMI_UI_VALUE_STRING_CAPACITY]; char buttons[UMI_UI_DIALOG_BUTTON_MAX][UMI_UI_TEXT_CAPACITY]; size_t button_count; } UmiUiDialogRequest;
typedef struct UmiUiDialogResult { uint64_t request_id; int accepted; int32_t button_index; char value[UMI_UI_VALUE_STRING_CAPACITY]; } UmiUiDialogResult;
typedef struct UmiUiDialogService UmiUiDialogService;
UmiStatus umi_ui_dialog_service_create(UmiUiDialogService **out_service);
void umi_ui_dialog_service_destroy(UmiUiDialogService *service);
UmiStatus umi_ui_dialog_request(UmiUiDialogService *service, const UmiUiDialogRequest *request, uint64_t *out_id);
UmiStatus umi_ui_dialog_next(UmiUiDialogService *service, UmiUiDialogRequest *out_request);
UmiStatus umi_ui_dialog_complete(UmiUiDialogService *service, const UmiUiDialogResult *result);
UmiStatus umi_ui_dialog_result(const UmiUiDialogService *service, uint64_t request_id, UmiUiDialogResult *out_result);
size_t umi_ui_dialog_pending_count(const UmiUiDialogService *service);

#ifdef __cplusplus
}
#endif

#endif
