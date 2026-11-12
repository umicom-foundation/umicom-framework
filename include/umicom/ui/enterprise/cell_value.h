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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent cell value data shared with callers of this public contract.
 */
typedef struct UmiUiEntCellValue { UmiUiEntValueKind kind; char text[UMI_UI_ENT_VALUE_CAPACITY]; int64_t integer_value; double real_value; int boolean_value; } UmiUiEntCellValue;
/**
 * Release or reset state held by ui ent cell value so the same storage can be reused
 * safely.
 */
/** Validate only the active value: terminated text, a known kind and a 0/1
 * boolean. Other fields are ignored. REAL retains ordinary C floating-point
 * equality semantics, including NaN comparing unequal. No state is changed. */
UmiStatus umi_ui_ent_cell_value_validate(const UmiUiEntCellValue *value);

/** Text replacement captures borrowed input before changing the cell. On any
 * error the complete cell is unchanged. Text must be readable through its
 * terminator or UMI_UI_ENT_VALUE_CAPACITY; no input is truncated. Equality
 * returns zero for malformed values. Serialize mutation through one owner. */
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


#ifdef __cplusplus
}
#endif
#endif
