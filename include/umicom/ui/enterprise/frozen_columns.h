/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/frozen_columns.h
 *
 * PURPOSE:
 *   Describe leading and trailing frozen-column regions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H
#define UMICOM_UI_ENTERPRISE_FROZEN_COLUMNS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntFrozenColumns {
    size_t leading_count;
    size_t trailing_count;
    size_t total_columns;
} UmiUiEntFrozenColumns;
UmiStatus umi_ui_ent_frozen_columns_init(UmiUiEntFrozenColumns *value);
int umi_ui_ent_frozen_columns_validate(const UmiUiEntFrozenColumns *value);
#ifdef __cplusplus
}
#endif

#endif
