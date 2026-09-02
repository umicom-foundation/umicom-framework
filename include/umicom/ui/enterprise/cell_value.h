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
/**
 * Represent the ui ent cell value data shared with callers of this public contract.
 */
typedef struct UmiUiEntCellValue { UmiUiEntValueKind kind; char text[UMI_UI_ENT_VALUE_CAPACITY]; int64_t integer_value; double real_value; int boolean_value; } UmiUiEntCellValue;
/**
 * Release or reset state held by ui ent cell value so the same storage can be reused
 * safely.
 */
void umi_ui_ent_cell_value_clear(UmiUiEntCellValue *value);
/**
 * Provide the ui ent cell value set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_cell_value_set_text(UmiUiEntCellValue *value,const char *text);
/**
 * Provide the ui ent cell value set integer operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_integer(UmiUiEntCellValue *value,int64_t number);
/**
 * Provide the ui ent cell value set real operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_real(UmiUiEntCellValue *value,double number);
/**
 * Provide the ui ent cell value set boolean operation used by this module and its client
 * applications.
 */
void umi_ui_ent_cell_value_set_boolean(UmiUiEntCellValue *value,int enabled);
/**
 * Provide the ui ent cell value equal operation used by this module and its client
 * applications.
 */
int umi_ui_ent_cell_value_equal(const UmiUiEntCellValue *left,const UmiUiEntCellValue *right);

#endif
