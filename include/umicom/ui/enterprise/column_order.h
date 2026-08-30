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
typedef struct UmiUiEntColumnOrder { char ids[UMI_UI_ENT_MAX_COLUMNS][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntColumnOrder;
void umi_ui_ent_column_order_init(UmiUiEntColumnOrder *o);
UmiStatus umi_ui_ent_column_order_append(UmiUiEntColumnOrder *o,const char *id);
UmiStatus umi_ui_ent_column_order_move(UmiUiEntColumnOrder *o,size_t from,size_t to);

#endif
