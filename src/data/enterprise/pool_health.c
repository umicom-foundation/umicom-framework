/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/pool_health.c
 *
 * PURPOSE:
 *   Summarise connection-pool capacity, utilisation and degraded/unavailable states.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/pool_health.h"
#include <string.h>

/* Pool health distinguishes full utilisation from unhealthy capacity loss. */ UmiStatus umi_data_pool_health_compute(const UmiDataConnectionPool *pool,UmiDataPoolHealth *out_health){size_t i;if(pool==NULL||out_health==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_health,0,sizeof(*out_health));out_health->total=pool->count;out_health->leased=pool->leased_count;for(i=0;i<pool->count;++i)if(pool->slots[i].healthy)out_health->healthy++;out_health->available=out_health->healthy>out_health->leased?out_health->healthy-out_health->leased:0U;out_health->utilisation_percent=umi_data_enterprise_percent((uint64_t)out_health->leased,(uint64_t)(out_health->healthy==0U?1U:out_health->healthy));if(out_health->healthy==0U)out_health->state=UMI_DATA_ENTERPRISE_FAILED;else if(out_health->available==0U)out_health->state=UMI_DATA_ENTERPRISE_DEGRADED;else out_health->state=UMI_DATA_ENTERPRISE_READY;return UMI_STATUS_OK;}
