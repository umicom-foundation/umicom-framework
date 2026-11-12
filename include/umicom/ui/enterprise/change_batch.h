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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent change batch data shared with callers of this public contract.
 */
typedef struct UmiUiEntChangeBatch { UmiUiEntDataDelta items[UMI_UI_ENT_MAX_RULES]; size_t count; uint64_t first_sequence; uint64_t last_sequence; } UmiUiEntChangeBatch;
/**
 * Initialise ui ent change batch from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_change_batch_init(UmiUiEntChangeBatch *b);
/**
 * Add ui ent change batch only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_ent_change_batch_add(UmiUiEntChangeBatch *b,const UmiUiEntDataDelta *delta);


#ifdef __cplusplus
}
#endif
#endif
