/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/cell_style.h
 *
 * PURPOSE:
 *   Describe semantic cell presentation independent of a renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CELL_STYLE_H
#define UMICOM_UI_ENTERPRISE_CELL_STYLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent cell style data shared with callers of this public contract.
 */
typedef struct UmiUiEntCellStyle {
    char semantic_role[UMI_UI_ENT_ID_CAPACITY];
    int bold;
    int italic;
    int32_t alignment;
    int32_t indent;
} UmiUiEntCellStyle;
/**
 * Initialise ui ent cell style from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_cell_style_init(UmiUiEntCellStyle *value);
/**
 * Check that ui ent cell style satisfies its contract before another service relies on it.
 */
int umi_ui_ent_cell_style_validate(const UmiUiEntCellStyle *value);
#ifdef __cplusplus
}
#endif

#endif
