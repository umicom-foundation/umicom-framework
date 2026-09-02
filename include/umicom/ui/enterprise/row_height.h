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
/**
 * Represent the ui ent row height data shared with callers of this public contract.
 */
typedef struct UmiUiEntRowHeight {
    int32_t preferred;
    int32_t minimum;
    int32_t maximum;
    int automatic;
} UmiUiEntRowHeight;
/**
 * Initialise ui ent row height from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_row_height_init(UmiUiEntRowHeight *value);
/**
 * Check that ui ent row height satisfies its contract before another service relies on it.
 */
int umi_ui_ent_row_height_validate(const UmiUiEntRowHeight *value);
#ifdef __cplusplus
}
#endif

#endif
