/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_health.h
 *
 * PURPOSE:
 *   Combine pool, replication, migration and integrity evidence into one Data Server health projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_HEALTH_H
#define UMICOM_DATA_ENTERPRISE_DATA_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataHealth { UmiDataEnterpriseState pool_state; UmiDataEnterpriseState replication_state; UmiDataEnterpriseState integrity_state; bool migration_blocked; UmiDataEnterpriseState overall_state; size_t blockers; } UmiDataHealth;
/* Compute aggregate health while preserving each subsystem state. */ UmiStatus umi_data_health_compute(UmiDataHealth *health,UmiDataEnterpriseState pool_state,UmiDataEnterpriseState replication_state,UmiDataEnterpriseState integrity_state,bool migration_blocked);

#ifdef __cplusplus
}
#endif
#endif
