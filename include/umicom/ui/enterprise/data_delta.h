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
typedef enum UmiUiEntDeltaKind { UMI_UI_ENT_DELTA_INSERT=1, UMI_UI_ENT_DELTA_REMOVE=2, UMI_UI_ENT_DELTA_UPDATE=3, UMI_UI_ENT_DELTA_RESET=4 } UmiUiEntDeltaKind;
typedef struct UmiUiEntDataDelta { UmiUiEntDeltaKind kind; UmiUiEntSpan rows; uint64_t sequence; } UmiUiEntDataDelta;
int umi_ui_ent_data_delta_validate(const UmiUiEntDataDelta *delta);
int umi_ui_ent_data_delta_touches(const UmiUiEntDataDelta *delta,size_t row);

#endif
