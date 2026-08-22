/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field.h
 *
 * PURPOSE:
 *   Define named typed fields carried by structured workbench selections.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionField {
    char name[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiWorkbenchSelectionValueKind kind;
    char text[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    int64_t integer_value;
    uint64_t unsigned_value;
    double decimal_value;
    bool boolean_value;
} UmiWorkbenchSelectionField;

void umi_workbench_selection_field_init(
    UmiWorkbenchSelectionField *field,
    const char *name);
UmiStatus umi_workbench_selection_field_set_text(
    UmiWorkbenchSelectionField *field,
    const char *value);
UmiStatus umi_workbench_selection_field_set_integer(
    UmiWorkbenchSelectionField *field,
    int64_t value);
UmiStatus umi_workbench_selection_field_set_unsigned(
    UmiWorkbenchSelectionField *field,
    uint64_t value);
UmiStatus umi_workbench_selection_field_set_decimal(
    UmiWorkbenchSelectionField *field,
    double value);
UmiStatus umi_workbench_selection_field_set_boolean(
    UmiWorkbenchSelectionField *field,
    bool value);
UmiStatus umi_workbench_selection_field_validate(
    const UmiWorkbenchSelectionField *field);

#ifdef __cplusplus
}
#endif
#endif
