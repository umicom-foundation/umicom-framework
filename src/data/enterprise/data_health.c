/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_health.c
 *
 * PURPOSE:
 *   Combine pool, replication, migration and integrity evidence into one Data Server health projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_health.h"
#include <string.h>

/* Aggregate health uses worst-state dominance and exposes blocker count for operations dashboards. */ UmiStatus umi_data_health_compute(UmiDataHealth *health,UmiDataEnterpriseState pool_state,UmiDataEnterpriseState replication_state,UmiDataEnterpriseState integrity_state,bool migration_blocked){UmiDataEnterpriseState worst;if(health==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(health,0,sizeof(*health));health->pool_state=pool_state;health->replication_state=replication_state;health->integrity_state=integrity_state;health->migration_blocked=migration_blocked;worst=pool_state;if(replication_state>worst)worst=replication_state;if(integrity_state>worst)worst=integrity_state;if(migration_blocked){health->blockers++;if(worst<UMI_DATA_ENTERPRISE_BLOCKED)worst=UMI_DATA_ENTERPRISE_BLOCKED;}if(pool_state==UMI_DATA_ENTERPRISE_FAILED)health->blockers++;if(replication_state==UMI_DATA_ENTERPRISE_FAILED)health->blockers++;if(integrity_state==UMI_DATA_ENTERPRISE_FAILED)health->blockers++;health->overall_state=worst;return UMI_STATUS_OK;}
