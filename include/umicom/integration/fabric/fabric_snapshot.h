/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/fabric_snapshot.h
 *
 * PURPOSE:
 *   Capture deterministic Integration Fabric inventory and health evidence for CLI, Desk and operations surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiFabricSnapshot { size_t service_contracts; size_t service_instances; size_t endpoints; size_t connectors; size_t api_operations; size_t routes; UmiFabricState health; uint64_t revision; uint64_t fingerprint; } UmiFabricSnapshot;
UmiStatus umi_fabric_snapshot_build(size_t service_contracts,size_t service_instances,size_t endpoints,size_t connectors,size_t api_operations,size_t routes,UmiFabricState health,uint64_t revision,UmiFabricSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
