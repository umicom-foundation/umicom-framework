/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_template.h
 *
 * PURPOSE:
 *   Define reusable named layout templates composed from semantic workstation surfaces and dock regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_TEMPLATE_H
#define UMICOM_UI_WORKSTATION_LAYOUT_TEMPLATE_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsLayoutItem {
    char surface_id[UMI_UI_ID_CAPACITY];
    UmiWsDockRegion region;
    int32_t row;
    int32_t column;
    int32_t row_span;
    int32_t column_span;
    double weight;
    bool visible;
} UmiWsLayoutItem;

typedef struct UmiWsLayoutTemplate {
    char layout_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char perspective_id[UMI_UI_ID_CAPACITY];
    UmiWsApplicationDomain domain;
    UmiWsLayoutVariantKind variant;
    UmiWsLayoutItem items[UMI_WS_MAX_LAYOUT_ITEMS];
    size_t count;
    bool locked_by_default;
} UmiWsLayoutTemplate;

UmiStatus umi_ws_layout_template_init(UmiWsLayoutTemplate *layout,
                                      const char *layout_id,
                                      const char *label,
                                      UmiWsApplicationDomain domain);
UmiStatus umi_ws_layout_template_add(UmiWsLayoutTemplate *layout,
                                     const char *surface_id,
                                     UmiWsDockRegion region,
                                     int32_t row,
                                     int32_t column,
                                     double weight);
const UmiWsLayoutItem *umi_ws_layout_template_find(const UmiWsLayoutTemplate *layout,
                                                   const char *surface_id);
uint64_t umi_ws_layout_template_fingerprint(const UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
