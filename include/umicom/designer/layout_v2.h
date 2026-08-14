/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/layout_v2.h
 *
 * PURPOSE:
 *   Apply alignment, distribution and grid layout as undoable transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Layout calculations modify semantic rectangles. Frontends draw the result but
 * do not own separate geometry, so generated source and live preview stay equal.
 */
#ifndef UMICOM_DESIGNER_LAYOUT_V2_H
#define UMICOM_DESIGNER_LAYOUT_V2_H

#include "umicom/designer/selection.h"
#include "umicom/designer/surface.h"
#include "umicom/designer/transaction_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_designer_layout_v2_align(UmiDesignerDocument *document,
                                       UmiDesignerTransactionHistoryV2 *history,
                                       const UmiDesignerSelection *selection,
                                       UmiDesignerAlignment alignment);
UmiStatus umi_designer_layout_v2_distribute(UmiDesignerDocument *document,
                                            UmiDesignerTransactionHistoryV2 *history,
                                            const UmiDesignerSelection *selection,
                                            UmiDesignerDistribution distribution);
UmiStatus umi_designer_layout_v2_snap(UmiDesignerDocument *document,
                                      UmiDesignerTransactionHistoryV2 *history,
                                      const UmiDesignerSelection *selection,
                                      const UmiDesignerSurfaceOptions *options);

#ifdef __cplusplus
}
#endif
#endif
