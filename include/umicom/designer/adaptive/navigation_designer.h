/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/navigation_designer.h
 *
 * PURPOSE:
 *   Own adaptive navigation-model selection and reorder operations for visual authoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_NAVIGATION_DESIGNER_H
#define UMICOM_DESIGNER_ADAPTIVE_NAVIGATION_DESIGNER_H
#include "umicom/designer/adaptive/types.h"
#include "umicom/ui/adaptive/navigation_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerNavigationDesigner {
    UmiAdaptiveNavigationModel model;
    char selected_item_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    uint64_t revision;
} UmiDesignerNavigationDesigner;
/* Initialise navigation designer state from one semantic model. */
UmiStatus umi_designer_navigation_designer_init(UmiDesignerNavigationDesigner *designer,
                                                const UmiAdaptiveNavigationModel *model);
/* Move one navigation item to a new bounded position. */
UmiStatus umi_designer_navigation_designer_move(UmiDesignerNavigationDesigner *designer,
                                                size_t from_index,
                                                size_t to_index);

#ifdef __cplusplus
}
#endif
#endif
