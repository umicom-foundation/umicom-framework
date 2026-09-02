/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/data_delta.h
 *
 * PURPOSE:
 *   Represent inserted, removed, updated or reset row ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_DATA_DELTA_H
#define UMICOM_UI_ENTERPRISE_DATA_DELTA_H

#include "umicom/ui/enterprise/types.h"
/**
 * List the named ui ent delta kind values accepted by this public contract.
 */
typedef enum UmiUiEntDeltaKind { UMI_UI_ENT_DELTA_INSERT=1, UMI_UI_ENT_DELTA_REMOVE=2, UMI_UI_ENT_DELTA_UPDATE=3, UMI_UI_ENT_DELTA_RESET=4 } UmiUiEntDeltaKind;
/**
 * Represent the ui ent data delta data shared with callers of this public contract.
 */
typedef struct UmiUiEntDataDelta { UmiUiEntDeltaKind kind; UmiUiEntSpan rows; uint64_t sequence; } UmiUiEntDataDelta;
/**
 * Check that ui ent data delta satisfies its contract before another service relies on it.
 */
int umi_ui_ent_data_delta_validate(const UmiUiEntDataDelta *delta);
/**
 * Provide the ui ent data delta touches operation used by this module and its client
 * applications.
 */
int umi_ui_ent_data_delta_touches(const UmiUiEntDataDelta *delta,size_t row);

#endif
