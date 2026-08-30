/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral primitives for declarative UI binding and reactive state graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_TYPES_H
#define UMICOM_UI_REACTIVE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ui/value.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UI_REACTIVE_ID_CAPACITY 128U
#define UMI_UI_REACTIVE_PATH_CAPACITY 256U
#define UMI_UI_REACTIVE_TEXT_CAPACITY 512U
#define UMI_UI_REACTIVE_MAX_ITEMS 256U
#define UMI_UI_REACTIVE_MAX_EDGES 512U
#define UMI_UI_REACTIVE_MAX_TOKENS 128U

typedef enum UmiUiReactiveBindingDirection {
    UMI_UI_REACTIVE_ONE_WAY = 1,
    UMI_UI_REACTIVE_ONE_WAY_TO_SOURCE = 2,
    UMI_UI_REACTIVE_TWO_WAY = 3,
    UMI_UI_REACTIVE_ONE_TIME = 4
} UmiUiReactiveBindingDirection;

typedef enum UmiUiReactiveUpdateTrigger {
    UMI_UI_REACTIVE_ON_CHANGE = 1,
    UMI_UI_REACTIVE_ON_COMMIT = 2,
    UMI_UI_REACTIVE_EXPLICIT = 3
} UmiUiReactiveUpdateTrigger;

typedef enum UmiUiReactiveValidationSeverity {
    UMI_UI_REACTIVE_VALIDATION_INFO = 1,
    UMI_UI_REACTIVE_VALIDATION_WARNING = 2,
    UMI_UI_REACTIVE_VALIDATION_ERROR = 3
} UmiUiReactiveValidationSeverity;

typedef enum UmiUiReactiveTransactionState {
    UMI_UI_REACTIVE_TRANSACTION_IDLE = 0,
    UMI_UI_REACTIVE_TRANSACTION_ACTIVE = 1,
    UMI_UI_REACTIVE_TRANSACTION_COMMITTED = 2,
    UMI_UI_REACTIVE_TRANSACTION_ROLLED_BACK = 3
} UmiUiReactiveTransactionState;

typedef struct UmiUiReactiveKeyValue {
    char key[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
} UmiUiReactiveKeyValue;

int umi_ui_reactive_identifier_valid(const char *text);
const char *umi_ui_reactive_binding_direction_name(UmiUiReactiveBindingDirection direction);
#ifdef __cplusplus
}
#endif
#endif
