/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/unit_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded catalogue of physical commodity units.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/unit_catalogue.h"

#include <string.h>

/* Reset catalogue state without heap allocation. */
void umi_commodity_unit_catalogue_init(UmiCommodityUnitCatalogue *catalogue)
{
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof *catalogue);
    }
}

/* Reject duplicates and preserve insertion order for reproducible snapshots. */
UmiStatus umi_commodity_unit_catalogue_add(UmiCommodityUnitCatalogue *catalogue, const UmiCommodityUnitOfMeasure *item)
{
    size_t index;
    if (catalogue == NULL || item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].code, (*item).code) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (catalogue->count >= UMI_COMMODITY_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *item;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/* Resolve reference data without exposing catalogue storage mutability. */
const UmiCommodityUnitOfMeasure *umi_commodity_unit_catalogue_find(const UmiCommodityUnitCatalogue *catalogue, const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) {
        return NULL;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].code, id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}
