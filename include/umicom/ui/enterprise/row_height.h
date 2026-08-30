/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_height.h
 *
 * PURPOSE:
 *   Describe fixed or adaptive row-height constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_HEIGHT_H
#define UMICOM_UI_ENTERPRISE_ROW_HEIGHT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntRowHeight {
    int32_t preferred;
    int32_t minimum;
    int32_t maximum;
    int automatic;
} UmiUiEntRowHeight;
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value);
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value);
#ifdef __cplusplus
}
#endif

#endif
