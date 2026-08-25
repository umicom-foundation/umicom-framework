/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_lease.c
 *
 * PURPOSE:
 *   Govern service-registration leases so stale instances are excluded deterministically.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_lease.h"
#include <string.h>
#include <limits.h>


UmiStatus umi_fabric_service_lease_init(UmiFabricServiceLease *lease,const char *instance_id,uint64_t now_ms,uint64_t ttl_ms) {
    UmiStatus s; if(lease==NULL || ttl_ms==0U || UINT64_MAX-now_ms<ttl_ms) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(lease,0,sizeof(*lease)); s=umi_fabric_copy_text(lease->instance_id,sizeof(lease->instance_id),instance_id); if(s!=UMI_STATUS_OK)return s;
    lease->expires_at_ms=now_ms+ttl_ms; return UMI_STATUS_OK;
}
UmiStatus umi_fabric_service_lease_renew(UmiFabricServiceLease *lease,uint64_t now_ms,uint64_t ttl_ms) {
    if(lease==NULL || ttl_ms==0U || UINT64_MAX-now_ms<ttl_ms) return UMI_STATUS_INVALID_ARGUMENT;
    lease->expires_at_ms=now_ms+ttl_ms; lease->renewals++; return UMI_STATUS_OK;
}
bool umi_fabric_service_lease_active(const UmiFabricServiceLease *lease,uint64_t now_ms) { return lease!=NULL && lease->instance_id[0]!='\0' && now_ms<lease->expires_at_ms; }
