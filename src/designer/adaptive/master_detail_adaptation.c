/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/master_detail_adaptation.c
 *
 * PURPOSE:
 *   Resolve master/detail designer composition between split and stacked responsive layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/master_detail_adaptation.h"

/* Keep compact screens stacked unless detail is primary, when it becomes an overlay. */
UmiDesignerMasterDetailMode umi_designer_master_detail_adaptation_resolve(
    UmiDesignSizeClass size_class,
    int detail_is_primary)
{
    if (size_class >= UMI_DESIGN_SIZE_EXPANDED) return UMI_DESIGNER_MASTER_DETAIL_SPLIT;
    return detail_is_primary != 0
        ? UMI_DESIGNER_MASTER_DETAIL_OVERLAY
        : UMI_DESIGNER_MASTER_DETAIL_STACKED;
}
