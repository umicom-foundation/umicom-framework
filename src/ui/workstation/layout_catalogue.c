/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded catalogue of reusable and user-customisable workstation layout templates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/layout_catalogue.h"

void umi_ws_layout_catalogue_init(UmiWsLayoutCatalogue *catalogue) {
    if (catalogue != NULL) *catalogue = (UmiWsLayoutCatalogue){0};
}

UmiStatus umi_ws_layout_catalogue_upsert(UmiWsLayoutCatalogue *catalogue, const UmiWsLayoutTemplate *layout) {
    size_t index;
    if (catalogue == NULL || layout == NULL || !umi_ws_id_valid(layout->layout_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->layouts[index].layout_id, layout->layout_id) == 0) {
            catalogue->layouts[index] = *layout;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    if (catalogue->count >= UMI_WS_MAX_LAYOUTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->layouts[catalogue->count++] = *layout;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

const UmiWsLayoutTemplate *umi_ws_layout_catalogue_find(const UmiWsLayoutCatalogue *catalogue, const char *layout_id) {
    size_t index;
    if (catalogue == NULL || !umi_ws_id_valid(layout_id)) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->layouts[index].layout_id, layout_id) == 0) return &catalogue->layouts[index];
    }
    return NULL;
}

size_t umi_ws_layout_catalogue_count_domain(const UmiWsLayoutCatalogue *catalogue, UmiWsApplicationDomain domain) {
    size_t index;
    size_t count = 0U;
    if (catalogue == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index) if (catalogue->layouts[index].domain == domain) ++count;
    return count;
}
