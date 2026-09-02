/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/dialog.h
 *
 * PURPOSE:
 *   Define asynchronous, toolkit-neutral dialog requests and results that frontend
 *   adapters can present without leaking native widget handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named ui dialog kind values accepted by this public contract.
 */
typedef enum UmiUiDialogKind { UMI_UI_DIALOG_MESSAGE=1, UMI_UI_DIALOG_CONFIRM=2, UMI_UI_DIALOG_INPUT=3, UMI_UI_DIALOG_FILE=4 } UmiUiDialogKind;
/**
 * Represent the ui dialog request data shared with callers of this public contract.
 */
typedef struct UmiUiDialogRequest { uint64_t request_id; UmiUiDialogKind kind; UmiUiSeverity severity; char title[UMI_UI_TEXT_CAPACITY]; char message[UMI_UI_DESCRIPTION_CAPACITY]; char default_value[UMI_UI_VALUE_STRING_CAPACITY]; char buttons[UMI_UI_DIALOG_BUTTON_MAX][UMI_UI_TEXT_CAPACITY]; size_t button_count; } UmiUiDialogRequest;
/**
 * Represent the ui dialog result data shared with callers of this public contract.
 */
typedef struct UmiUiDialogResult { uint64_t request_id; int accepted; int32_t button_index; char value[UMI_UI_VALUE_STRING_CAPACITY]; } UmiUiDialogResult;
/**
 * Represent the ui dialog service data shared with callers of this public contract.
 */
typedef struct UmiUiDialogService UmiUiDialogService;
/**
 * Initialise ui dialog service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_dialog_service_create(UmiUiDialogService **out_service);
/**
 * Release or reset state held by ui dialog service so the same storage can be reused
 * safely.
 */
void umi_ui_dialog_service_destroy(UmiUiDialogService *service);
/**
 * Provide the ui dialog request operation used by this module and its client applications.
 */
UmiStatus umi_ui_dialog_request(UmiUiDialogService *service, const UmiUiDialogRequest *request, uint64_t *out_id);
/**
 * Provide the ui dialog next operation used by this module and its client applications.
 */
UmiStatus umi_ui_dialog_next(UmiUiDialogService *service, UmiUiDialogRequest *out_request);
/**
 * Provide the ui dialog complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_dialog_complete(UmiUiDialogService *service, const UmiUiDialogResult *result);
/**
 * Provide the ui dialog result operation used by this module and its client applications.
 */
UmiStatus umi_ui_dialog_result(const UmiUiDialogService *service, uint64_t request_id, UmiUiDialogResult *out_result);
/**
 * Return the number of records represented by ui dialog pending without changing their
 * state.
 */
size_t umi_ui_dialog_pending_count(const UmiUiDialogService *service);

#ifdef __cplusplus
}
#endif

#endif
