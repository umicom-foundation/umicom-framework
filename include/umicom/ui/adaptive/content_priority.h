/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/content_priority.h
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

#ifndef UMICOM_UI_ADAPTIVE_CONTENT_PRIORITY_H
#define UMICOM_UI_ADAPTIVE_CONTENT_PRIORITY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveContentPriority {
    char content_id[UMI_ADAPTIVE_ID_CAPACITY];
    uint32_t priority;
    int essential;
} UmiAdaptiveContentPriority;
/* Initialise semantic content-priority metadata. */
UmiStatus umi_adaptive_content_priority_init(UmiAdaptiveContentPriority *item,
                                             const char *content_id,
                                             uint32_t priority,
                                             int essential);
/* Compare two content-priority records for deterministic descending placement. */
int umi_adaptive_content_priority_compare(const UmiAdaptiveContentPriority *left,
                                          const UmiAdaptiveContentPriority *right);

#ifdef __cplusplus
}
#endif
#endif
