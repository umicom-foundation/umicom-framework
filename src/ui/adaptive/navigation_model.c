/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/navigation_model.c
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
#include "umicom/ui/adaptive/navigation_model.h"
#include <string.h>

/* Reset the bounded navigation collection. */
void umi_adaptive_navigation_model_init(UmiAdaptiveNavigationModel *model)
{
    if (model != NULL) memset(model, 0, sizeof *model);
}

/* Add one unique item while preserving caller-defined order. */
UmiStatus umi_adaptive_navigation_model_add(UmiAdaptiveNavigationModel *model,
                                            const UmiAdaptiveNavigationItem *item)
{
    if (model == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_adaptive_navigation_model_find(model, item->item_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (model->count >= UMI_ADAPTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->items[model->count++] = *item;
    return UMI_STATUS_OK;
}

/* Find an item by id for command routing and adaptation. */
const UmiAdaptiveNavigationItem *umi_adaptive_navigation_model_find(const UmiAdaptiveNavigationModel *model,
                                                                    const char *item_id)
{
    size_t index;
    if (model == NULL || item_id == NULL) return NULL;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].item_id, item_id) == 0) return &model->items[index];
    }
    return NULL;
}
