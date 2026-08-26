/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_group.h
 *
 * PURPOSE:
 *   Aggregate validation results for one form, object or editing transaction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_GROUP_H
#define UMICOM_UI_REACTIVE_VALIDATION_GROUP_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidationGroup {
    size_t total;
    size_t invalid;
    size_t warnings;
    bool blocking;
} UmiUiReactiveValidationGroup;
void umi_ui_reactive_validation_group_init(UmiUiReactiveValidationGroup *item);
int umi_ui_reactive_validation_group_valid(const UmiUiReactiveValidationGroup *item);
#ifdef __cplusplus
}
#endif
#endif
