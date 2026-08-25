/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/perspective_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded perspective catalogue so products can contribute task semantics without owning layout infrastructure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/perspective_catalogue.h"

void umi_ws_perspective_catalogue_init(UmiWsPerspectiveCatalogue *catalogue) {
    if (catalogue != NULL) *catalogue = (UmiWsPerspectiveCatalogue){0};
}

UmiStatus umi_ws_perspective_catalogue_upsert(UmiWsPerspectiveCatalogue *catalogue,
                                               const UmiWsPerspectiveTemplate *perspective) {
    size_t index;
    if (catalogue == NULL || perspective == NULL || !umi_ws_id_valid(perspective->perspective_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].perspective_id, perspective->perspective_id) == 0) {
            catalogue->items[index] = *perspective;
            return UMI_STATUS_OK;
        }
    }
    if (catalogue->count >= UMI_WS_MAX_PERSPECTIVES) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *perspective;
    return UMI_STATUS_OK;
}

const UmiWsPerspectiveTemplate *umi_ws_perspective_catalogue_find(const UmiWsPerspectiveCatalogue *catalogue,
                                                                  const char *perspective_id) {
    size_t index;
    if (catalogue == NULL || !umi_ws_id_valid(perspective_id)) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].perspective_id, perspective_id) == 0) return &catalogue->items[index];
    }
    return NULL;
}
