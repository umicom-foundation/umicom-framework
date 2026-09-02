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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) memset(model, 0, sizeof *model);
}

/* Add one unique item while preserving caller-defined order. */
UmiStatus umi_adaptive_navigation_model_add(UmiAdaptiveNavigationModel *model,
                                            const UmiAdaptiveNavigationItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_adaptive_navigation_model_find(model, item->item_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_ADAPTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->items[model->count++] = *item;
    return UMI_STATUS_OK;
}

/* Find an item by id for command routing and adaptation. */
const UmiAdaptiveNavigationItem *umi_adaptive_navigation_model_find(const UmiAdaptiveNavigationModel *model,
                                                                    const char *item_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].item_id, item_id) == 0) return &model->items[index];
    }
    return NULL;
}
