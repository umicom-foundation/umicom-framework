/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/pinned_rows.h
 *
 * PURPOSE:
 *   Track top and bottom pinned row identities without duplicating row data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_PINNED_ROWS_H
#define UMICOM_UI_ENTERPRISE_PINNED_ROWS_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntPinnedRows { uint64_t top[UMI_UI_ENT_MAX_SELECTIONS]; size_t top_count; uint64_t bottom[UMI_UI_ENT_MAX_SELECTIONS]; size_t bottom_count; } UmiUiEntPinnedRows;
void umi_ui_ent_pinned_rows_init(UmiUiEntPinnedRows *p);
UmiStatus umi_ui_ent_pinned_rows_add_top(UmiUiEntPinnedRows *p,uint64_t key);
UmiStatus umi_ui_ent_pinned_rows_add_bottom(UmiUiEntPinnedRows *p,uint64_t key);
int umi_ui_ent_pinned_rows_contains(const UmiUiEntPinnedRows *p,uint64_t key);

#endif
