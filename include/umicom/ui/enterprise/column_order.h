/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_order.h
 *
 * PURPOSE:
 *   Maintain a stable user-defined semantic column order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_ORDER_H
#define UMICOM_UI_ENTERPRISE_COLUMN_ORDER_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent column order data shared with callers of this public contract.
 */
typedef struct UmiUiEntColumnOrder { char ids[UMI_UI_ENT_MAX_COLUMNS][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntColumnOrder;
/**
 * Initialise ui ent column order from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_column_order_init(UmiUiEntColumnOrder *o);
/**
 * Add ui ent column order only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_ent_column_order_append(UmiUiEntColumnOrder *o,const char *id);
/**
 * Provide the ui ent column order move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_order_move(UmiUiEntColumnOrder *o,size_t from,size_t to);

#endif
