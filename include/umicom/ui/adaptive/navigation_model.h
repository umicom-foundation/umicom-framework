/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/navigation_model.h
 *
 * PURPOSE:
 *   Maintain a bounded canonical navigation model independent of renderer presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_NAVIGATION_MODEL_H
#define UMICOM_UI_ADAPTIVE_NAVIGATION_MODEL_H
#include "umicom/ui/adaptive/navigation_item.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveNavigationModel {
    UmiAdaptiveNavigationItem items[UMI_ADAPTIVE_MAX_ITEMS];
    size_t count;
} UmiAdaptiveNavigationModel;
/* Initialise an empty navigation model. */
void umi_adaptive_navigation_model_init(UmiAdaptiveNavigationModel *model);
/* Add a unique navigation item by value. */
UmiStatus umi_adaptive_navigation_model_add(UmiAdaptiveNavigationModel *model,
                                            const UmiAdaptiveNavigationItem *item);
/* Find one navigation item by stable identifier. */
const UmiAdaptiveNavigationItem *umi_adaptive_navigation_model_find(const UmiAdaptiveNavigationModel *model,
                                                                    const char *item_id);

#ifdef __cplusplus
}
#endif
#endif
