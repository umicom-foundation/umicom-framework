/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/connection_pool.h
 *
 * PURPOSE:
 *   Manage bounded connection-slot leasing, release and health without replacing backend connection creation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_CONNECTION_POOL_H
#define UMICOM_DATA_ENTERPRISE_CONNECTION_POOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/connection_slot.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataConnectionPool { UmiDataConnectionSlot slots[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t count; size_t leased_count; uint64_t revision; } UmiDataConnectionPool;
/* Reset connection-pool metadata. */ void umi_data_connection_pool_init(UmiDataConnectionPool *pool);
/* Register a backend-owned connection token as a pool slot. */ UmiStatus umi_data_connection_pool_add(UmiDataConnectionPool *pool,const UmiDataConnectionSlot *slot);
/* Lease the first healthy free slot and return its token. */ UmiStatus umi_data_connection_pool_acquire(UmiDataConnectionPool *pool,uint64_t now,uint64_t *out_token);
/* Release a previously leased backend token. */ UmiStatus umi_data_connection_pool_release(UmiDataConnectionPool *pool,uint64_t connection_token,uint64_t now);

#ifdef __cplusplus
}
#endif
#endif
