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

/**
 * Represent the fabric api catalogue data shared with callers of this public contract.
 */
typedef struct UmiFabricApiCatalogue {
    UmiFabricApiOperation items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricApiCatalogue;
/**
 * Initialise fabric api catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_fabric_api_catalogue_init(UmiFabricApiCatalogue *registry);
/**
 * Add fabric api catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fabric_api_catalogue_add(UmiFabricApiCatalogue *registry,const UmiFabricApiOperation *item);
/**
 * Find fabric api catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fabric_api_catalogue_find(const UmiFabricApiCatalogue *registry,const char *id,UmiFabricApiOperation *out_item);
/**
 * Return the number of records represented by fabric api catalogue without changing their
 * state.
 */
size_t umi_fabric_api_catalogue_count(const UmiFabricApiCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif
