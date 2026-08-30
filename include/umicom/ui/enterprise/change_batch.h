/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/change_batch.h
 *
 * PURPOSE:
 *   Collect ordered data deltas for one renderer update transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_CHANGE_BATCH_H
#define UMICOM_UI_ENTERPRISE_CHANGE_BATCH_H

#include "umicom/ui/enterprise/data_delta.h"
typedef struct UmiUiEntChangeBatch { UmiUiEntDataDelta items[UMI_UI_ENT_MAX_RULES]; size_t count; uint64_t first_sequence; uint64_t last_sequence; } UmiUiEntChangeBatch;
void umi_ui_ent_change_batch_init(UmiUiEntChangeBatch *b);
UmiStatus umi_ui_ent_change_batch_add(UmiUiEntChangeBatch *b,const UmiUiEntDataDelta *delta);

#endif
