/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/split_region.h
 *
 * PURPOSE:
 *   Define bounded split-region geometry for persistent two-pane workstation layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_SPLIT_REGION_H
#define UMICOM_UI_WORKSTATION_SPLIT_REGION_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsSplitRegion {
    char split_id[UMI_UI_ID_CAPACITY];
    char first_id[UMI_UI_ID_CAPACITY];
    char second_id[UMI_UI_ID_CAPACITY];
    UmiUiOrientation orientation;
    double ratio;
    double minimum_ratio;
    double maximum_ratio;
} UmiWsSplitRegion;

UmiStatus umi_ws_split_region_init(UmiWsSplitRegion *split,
                                   const char *split_id,
                                   UmiUiOrientation orientation,
                                   double ratio);
UmiStatus umi_ws_split_region_bind(UmiWsSplitRegion *split,
                                   const char *first_id,
                                   const char *second_id);
double umi_ws_split_region_set_ratio(UmiWsSplitRegion *split, double ratio);

#ifdef __cplusplus
}
#endif

#endif
