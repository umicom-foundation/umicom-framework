/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/api_catalogue.h
 *
 * PURPOSE:
 *   Maintain unique API operation contracts for gateway and connector routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_API_CATALOGUE_H
#define UMICOM_INTEGRATION_FABRIC_API_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/api_operation.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricApiCatalogue {
    UmiFabricApiOperation items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricApiCatalogue;
void umi_fabric_api_catalogue_init(UmiFabricApiCatalogue *registry);
UmiStatus umi_fabric_api_catalogue_add(UmiFabricApiCatalogue *registry,const UmiFabricApiOperation *item);
UmiStatus umi_fabric_api_catalogue_find(const UmiFabricApiCatalogue *registry,const char *id,UmiFabricApiOperation *out_item);
size_t umi_fabric_api_catalogue_count(const UmiFabricApiCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif
