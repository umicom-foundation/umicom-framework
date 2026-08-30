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
typedef struct UmiUiEntCellStyle {
    char semantic_role[UMI_UI_ENT_ID_CAPACITY];
    int bold;
    int italic;
    int32_t alignment;
    int32_t indent;
} UmiUiEntCellStyle;
UmiStatus umi_ui_ent_cell_style_init(UmiUiEntCellStyle *value);
int umi_ui_ent_cell_style_validate(const UmiUiEntCellStyle *value);
#ifdef __cplusplus
}
#endif

#endif
