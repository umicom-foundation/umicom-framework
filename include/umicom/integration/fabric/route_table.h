/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/route_table.h
 *
 * PURPOSE:
 *   Maintain unique routing rules whose priority is interpreted by the route matcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ROUTE_TABLE_H
#define UMICOM_INTEGRATION_FABRIC_ROUTE_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/route_rule.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric route table data shared with callers of this public contract.
 */
typedef struct UmiFabricRouteTable {
    UmiFabricRouteRule items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricRouteTable;
/**
 * Initialise fabric route table from caller-provided values so later operations receive a
 * known state.
 */
void umi_fabric_route_table_init(UmiFabricRouteTable *registry);
/**
 * Add fabric route table only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fabric_route_table_add(UmiFabricRouteTable *registry,const UmiFabricRouteRule *item);
/**
 * Find fabric route table while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fabric_route_table_find(const UmiFabricRouteTable *registry,const char *id,UmiFabricRouteRule *out_item);
/**
 * Return the number of records represented by fabric route table without changing their
 * state.
 */
size_t umi_fabric_route_table_count(const UmiFabricRouteTable *registry);

#ifdef __cplusplus
}
#endif
#endif
