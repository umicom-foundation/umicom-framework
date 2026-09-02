/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/pinned_rows.h
 *
 * PURPOSE:
 *   Track top and bottom pinned row identities without duplicating row data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_PINNED_ROWS_H
#define UMICOM_UI_ENTERPRISE_PINNED_ROWS_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent pinned rows data shared with callers of this public contract.
 */
typedef struct UmiUiEntPinnedRows { uint64_t top[UMI_UI_ENT_MAX_SELECTIONS]; size_t top_count; uint64_t bottom[UMI_UI_ENT_MAX_SELECTIONS]; size_t bottom_count; } UmiUiEntPinnedRows;
/**
 * Initialise ui ent pinned rows from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_pinned_rows_init(UmiUiEntPinnedRows *p);
/**
 * Provide the ui ent pinned rows add top operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_pinned_rows_add_top(UmiUiEntPinnedRows *p,uint64_t key);
/**
 * Provide the ui ent pinned rows add bottom operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_pinned_rows_add_bottom(UmiUiEntPinnedRows *p,uint64_t key);
/**
 * Provide the ui ent pinned rows contains operation used by this module and its client
 * applications.
 */
int umi_ui_ent_pinned_rows_contains(const UmiUiEntPinnedRows *p,uint64_t key);

#endif
