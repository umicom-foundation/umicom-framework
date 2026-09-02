/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/frozen_columns.h
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H
#define UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent frozen columns data shared with callers of this public contract.
 */
typedef struct UmiUiEntFrozenColumns {
    size_t leading_count;
    size_t trailing_count;
    size_t total_columns;
} UmiUiEntFrozenColumns;
/**
 * Initialise ui ent frozen columns from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value);
/**
 * Check that ui ent frozen columns satisfies its contract before another service relies on
 * it.
 */
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value);
#ifdef __cplusplus
}
#endif

#endif
