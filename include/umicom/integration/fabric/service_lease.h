/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_lease.h
 *
 * PURPOSE:
 *   Govern service-registration leases so stale instances are excluded deterministically.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_LEASE_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_LEASE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricServiceLease { char instance_id[UMI_FABRIC_ID_CAPACITY]; uint64_t expires_at_ms; uint32_t renewals; } UmiFabricServiceLease;
UmiStatus umi_fabric_service_lease_init(UmiFabricServiceLease *lease,const char *instance_id,uint64_t now_ms,uint64_t ttl_ms);
UmiStatus umi_fabric_service_lease_renew(UmiFabricServiceLease *lease,uint64_t now_ms,uint64_t ttl_ms);
bool umi_fabric_service_lease_active(const UmiFabricServiceLease *lease,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
