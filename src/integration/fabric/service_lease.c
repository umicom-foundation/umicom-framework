/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/service_lease.c
 *
 * PURPOSE:
 *   Govern service-registration leases so stale instances are excluded deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_lease.h"
#include <string.h>
#include <limits.h>


/*
 * Initialise fabric service lease from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_service_lease_init(UmiFabricServiceLease *lease,const char *instance_id,uint64_t now_ms,uint64_t ttl_ms) {
    UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(lease==NULL || ttl_ms==0U || UINT64_MAX-now_ms<ttl_ms) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(lease,0,sizeof(*lease)); s=umi_fabric_copy_text(lease->instance_id,sizeof(lease->instance_id),instance_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    lease->expires_at_ms=now_ms+ttl_ms; return UMI_STATUS_OK;
}
/*
 * Provide the fabric service lease renew operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_service_lease_renew(UmiFabricServiceLease *lease,uint64_t now_ms,uint64_t ttl_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(lease==NULL || ttl_ms==0U || UINT64_MAX-now_ms<ttl_ms) return UMI_STATUS_INVALID_ARGUMENT;
    lease->expires_at_ms=now_ms+ttl_ms; lease->renewals++; return UMI_STATUS_OK;
}
/*
 * Provide the fabric service lease active operation used by this module and its client
 * applications.
 */
bool umi_fabric_service_lease_active(const UmiFabricServiceLease *lease,uint64_t now_ms) { return lease!=NULL && lease->instance_id[0]!='\0' && now_ms<lease->expires_at_ms; }
