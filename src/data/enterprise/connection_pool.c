/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/connection_pool.c
 *
 * PURPOSE:
 *   Manage bounded connection-slot leasing, release and health without replacing backend connection creation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/connection_pool.h"
#include <string.h>

/* Pool reset does not close connections because token ownership belongs to backend adapters. */ void umi_data_connection_pool_init(UmiDataConnectionPool *pool){if(pool!=NULL)(void)memset(pool,0,sizeof(*pool));}
/* Registration rejects duplicate tokens and IDs. */ UmiStatus umi_data_connection_pool_add(UmiDataConnectionPool *pool,const UmiDataConnectionSlot *slot){size_t i;if(pool==NULL||slot==NULL||umi_data_connection_slot_validate(slot)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<pool->count;++i)if(pool->slots[i].connection_token==slot->connection_token||strcmp(pool->slots[i].slot_id,slot->slot_id)==0)return UMI_STATUS_ALREADY_EXISTS;if(pool->count>=UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;pool->slots[pool->count++]=*slot;pool->revision++;return UMI_STATUS_OK;}
/* Acquisition is deterministic; richer adapters may later layer wait queues or affinity. */ UmiStatus umi_data_connection_pool_acquire(UmiDataConnectionPool *pool,uint64_t now,uint64_t *out_token){size_t i;if(pool==NULL||out_token==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<pool->count;++i)if(pool->slots[i].healthy&&!pool->slots[i].leased){pool->slots[i].leased=true;pool->slots[i].lease_count++;pool->slots[i].last_used_at=now;pool->leased_count++;pool->revision++;*out_token=pool->slots[i].connection_token;return UMI_STATUS_OK;}return UMI_STATUS_BUSY;}
/* Release verifies lease state and keeps usage evidence for pool-health projections. */ UmiStatus umi_data_connection_pool_release(UmiDataConnectionPool *pool,uint64_t connection_token,uint64_t now){size_t i;if(pool==NULL||connection_token==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<pool->count;++i)if(pool->slots[i].connection_token==connection_token){if(!pool->slots[i].leased)return UMI_STATUS_INVALID_STATE;pool->slots[i].leased=false;pool->slots[i].last_used_at=now;if(pool->leased_count>0U)pool->leased_count--;pool->revision++;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
