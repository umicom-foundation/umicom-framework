/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/content_priority.c
 *
 * PURPOSE:
 *   Represent semantic content priority and compare items for constrained adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/content_priority.h"
#include <string.h>

/* Store priority and essentiality without coupling to any concrete component. */
UmiStatus umi_adaptive_content_priority_init(UmiAdaptiveContentPriority *item,
                                             const char *content_id,
                                             uint32_t priority,
                                             int essential)
{
    if (item == NULL || content_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    if (umi_adaptive_copy_text(item->content_id, sizeof item->content_id, content_id) != UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    item->priority = priority;
    item->essential = essential != 0;
    return UMI_STATUS_OK;
}

/* Rank essential content first, then larger numeric priority, then stable id. */
int umi_adaptive_content_priority_compare(const UmiAdaptiveContentPriority *left,
                                          const UmiAdaptiveContentPriority *right)
{
    if (left == NULL || right == NULL) return 0;
    if (left->essential != right->essential) return left->essential ? -1 : 1;
    if (left->priority != right->priority) return left->priority > right->priority ? -1 : 1;
    return strcmp(left->content_id, right->content_id);
}
