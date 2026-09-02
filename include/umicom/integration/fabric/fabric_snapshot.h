/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/fabric_snapshot.h
 *
 * PURPOSE:
 *   Capture deterministic Integration Fabric inventory and health evidence for CLI, Desk and operations surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_FABRIC_SNAPSHOT_H
#define UMICOM_INTEGRATION_FABRIC_FABRIC_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric snapshot data shared with callers of this public contract.
 */
typedef struct UmiFabricSnapshot { size_t service_contracts; size_t service_instances; size_t endpoints; size_t connectors; size_t api_operations; size_t routes; UmiFabricState health; uint64_t revision; uint64_t fingerprint; } UmiFabricSnapshot;
/**
 * Provide the fabric snapshot build operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_snapshot_build(size_t service_contracts,size_t service_instances,size_t endpoints,size_t connectors,size_t api_operations,size_t routes,UmiFabricState health,uint64_t revision,UmiFabricSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
