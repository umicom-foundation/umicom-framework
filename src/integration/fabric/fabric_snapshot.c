/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/fabric_snapshot.c
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
#include "umicom/integration/fabric/fabric_snapshot.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_snapshot_build(size_t sc,size_t si,size_t ep,size_t co,size_t api,size_t routes,UmiFabricState health,uint64_t revision,UmiFabricSnapshot *out){if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));out->service_contracts=sc;out->service_instances=si;out->endpoints=ep;out->connectors=co;out->api_operations=api;out->routes=routes;out->health=health;out->revision=revision;out->fingerprint=umi_fabric_hash64(out,offsetof(UmiFabricSnapshot,fingerprint),0U);return UMI_STATUS_OK;}
