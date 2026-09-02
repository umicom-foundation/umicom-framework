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

/**
 * List the named ui reactive binding direction values accepted by this public contract.
 */
typedef enum UmiUiReactiveBindingDirection {
    UMI_UI_REACTIVE_ONE_WAY = 1,
    UMI_UI_REACTIVE_ONE_WAY_TO_SOURCE = 2,
    UMI_UI_REACTIVE_TWO_WAY = 3,
    UMI_UI_REACTIVE_ONE_TIME = 4
} UmiUiReactiveBindingDirection;

/**
 * List the named ui reactive update trigger values accepted by this public contract.
 */
typedef enum UmiUiReactiveUpdateTrigger {
    UMI_UI_REACTIVE_ON_CHANGE = 1,
    UMI_UI_REACTIVE_ON_COMMIT = 2,
    UMI_UI_REACTIVE_EXPLICIT = 3
} UmiUiReactiveUpdateTrigger;

/**
 * List the named ui reactive validation severity values accepted by this public contract.
 */
typedef enum UmiUiReactiveValidationSeverity {
    UMI_UI_REACTIVE_VALIDATION_INFO = 1,
    UMI_UI_REACTIVE_VALIDATION_WARNING = 2,
    UMI_UI_REACTIVE_VALIDATION_ERROR = 3
} UmiUiReactiveValidationSeverity;

/**
 * List the named ui reactive transaction state values accepted by this public contract.
 */
typedef enum UmiUiReactiveTransactionState {
    UMI_UI_REACTIVE_TRANSACTION_IDLE = 0,
    UMI_UI_REACTIVE_TRANSACTION_ACTIVE = 1,
    UMI_UI_REACTIVE_TRANSACTION_COMMITTED = 2,
    UMI_UI_REACTIVE_TRANSACTION_ROLLED_BACK = 3
} UmiUiReactiveTransactionState;

/**
 * Represent the ui reactive key value data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveKeyValue {
    char key[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
} UmiUiReactiveKeyValue;

/**
 * Check that ui reactive identifier satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_identifier_valid(const char *text);
/**
 * Provide the ui reactive binding direction name operation used by this module and its
 * client applications.
 */
const char *umi_ui_reactive_binding_direction_name(UmiUiReactiveBindingDirection direction);
#ifdef __cplusplus
}
#endif
#endif
