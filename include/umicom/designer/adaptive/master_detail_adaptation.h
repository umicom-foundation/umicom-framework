/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/master_detail_adaptation.h
 *
 * PURPOSE:
 *   Resolve master/detail designer composition between split and stacked responsive layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_MASTER_DETAIL_ADAPTATION_H
#define UMICOM_DESIGNER_ADAPTIVE_MASTER_DETAIL_ADAPTATION_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignerMasterDetailMode {
    UMI_DESIGNER_MASTER_DETAIL_STACKED = 1,
    UMI_DESIGNER_MASTER_DETAIL_SPLIT = 2,
    UMI_DESIGNER_MASTER_DETAIL_OVERLAY = 3
} UmiDesignerMasterDetailMode;
/* Choose master/detail composition from canonical size class and detail priority. */
UmiDesignerMasterDetailMode umi_designer_master_detail_adaptation_resolve(
    UmiDesignSizeClass size_class,
    int detail_is_primary);

#ifdef __cplusplus
}
#endif
#endif
