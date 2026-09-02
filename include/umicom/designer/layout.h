/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/layout.h
 *
 * PURPOSE:
 *   Apply alignment, distribution and grid layout as undoable transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Layout calculations modify semantic rectangles. Frontends draw the result but
 * do not own separate geometry, so generated source and live preview stay equal.
 */
#ifndef UMICOM_DESIGNER_LAYOUT_H
#define UMICOM_DESIGNER_LAYOUT_H

#include "umicom/designer/selection.h"
#include "umicom/designer/surface.h"
#include "umicom/designer/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the designer layout align operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_layout_align(UmiDesignerDocument *document,
                                       UmiDesignerTransactionHistory *history,
                                       const UmiDesignerSelection *selection,
                                       UmiDesignerAlignment alignment);
/**
 * Provide the designer layout distribute operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_layout_distribute(UmiDesignerDocument *document,
                                            UmiDesignerTransactionHistory *history,
                                            const UmiDesignerSelection *selection,
                                            UmiDesignerDistribution distribution);
/**
 * Provide the designer layout snap operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_layout_snap(UmiDesignerDocument *document,
                                      UmiDesignerTransactionHistory *history,
                                      const UmiDesignerSelection *selection,
                                      const UmiDesignerSurfaceOptions *options);

#ifdef __cplusplus
}
#endif
#endif
