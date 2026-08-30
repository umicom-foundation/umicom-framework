/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/cell_value.h
 *
 * PURPOSE:
 *   Store a typed cell value without toolkit or domain-specific ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CELL_VALUE_H
#define UMICOM_UI_ENTERPRISE_CELL_VALUE_H

#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntCellValue { UmiUiEntValueKind kind; char text[UMI_UI_ENT_VALUE_CAPACITY]; int64_t integer_value; double real_value; int boolean_value; } UmiUiEntCellValue;
void umi_ui_ent_cell_value_clear(UmiUiEntCellValue *value);
UmiStatus umi_ui_ent_cell_value_set_text(UmiUiEntCellValue *value,const char *text);
void umi_ui_ent_cell_value_set_integer(UmiUiEntCellValue *value,int64_t number);
void umi_ui_ent_cell_value_set_real(UmiUiEntCellValue *value,double number);
void umi_ui_ent_cell_value_set_boolean(UmiUiEntCellValue *value,int enabled);
int umi_ui_ent_cell_value_equal(const UmiUiEntCellValue *left,const UmiUiEntCellValue *right);

#endif
