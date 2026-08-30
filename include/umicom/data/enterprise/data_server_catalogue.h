/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_server_catalogue.h
 *
 * PURPOSE:
 *   Maintain reusable Data Server deployment profiles for applications and environments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_SERVER_CATALOGUE_H
#define UMICOM_DATA_ENTERPRISE_DATA_SERVER_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/data_server_profile.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataServerCatalogue {
    UmiDataServerProfile items[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDataServerCatalogue;

/* Reset the bounded data server catalogue registry. */
void umi_data_data_server_catalogue_init(UmiDataServerCatalogue *registry);
/* Add a unique item by stable identifier. */
UmiStatus umi_data_data_server_catalogue_add(UmiDataServerCatalogue *registry, const UmiDataServerProfile *item);
/* Find a registry item by stable identifier. */
UmiStatus umi_data_data_server_catalogue_find(const UmiDataServerCatalogue *registry, const char *id, UmiDataServerProfile *out_item);
/* Return the number of registered items. */
size_t umi_data_data_server_catalogue_count(const UmiDataServerCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif
