/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_lease.h
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


/**
 * Represent the fabric service lease data shared with callers of this public contract.
 */
typedef struct UmiFabricServiceLease { char instance_id[UMI_FABRIC_ID_CAPACITY]; uint64_t expires_at_ms; uint32_t renewals; } UmiFabricServiceLease;
/**
 * Initialise fabric service lease from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_service_lease_init(UmiFabricServiceLease *lease,const char *instance_id,uint64_t now_ms,uint64_t ttl_ms);
/**
 * Provide the fabric service lease renew operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_service_lease_renew(UmiFabricServiceLease *lease,uint64_t now_ms,uint64_t ttl_ms);
/**
 * Provide the fabric service lease active operation used by this module and its client
 * applications.
 */
bool umi_fabric_service_lease_active(const UmiFabricServiceLease *lease,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif
