/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/cell_validation.h
 *
 * PURPOSE:
 *   Apply reusable required, length and numeric-range constraints to cell values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CELL_VALIDATION_H
#define UMICOM_UI_ENTERPRISE_CELL_VALIDATION_H

#include "umicom/ui/enterprise/cell_value.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent cell validation rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntCellValidationRule { int required; size_t max_length; int has_minimum; int has_maximum; double minimum; double maximum; } UmiUiEntCellValidationRule;
/**
 * Check that ui ent cell validation satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ui_ent_cell_validation_validate(const UmiUiEntCellValidationRule *rule,const UmiUiEntCellValue *value,char *message,size_t message_capacity);


#ifdef __cplusplus
}
#endif
#endif
