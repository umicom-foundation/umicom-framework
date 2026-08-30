/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/pool_health.h
 *
 * PURPOSE:
 *   Summarise connection-pool capacity, utilisation and degraded/unavailable states.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_POOL_HEALTH_H
#define UMICOM_DATA_ENTERPRISE_POOL_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/connection_pool.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataPoolHealth { size_t total; size_t healthy; size_t leased; size_t available; double utilisation_percent; UmiDataEnterpriseState state; } UmiDataPoolHealth;
/* Compute pool health from slot evidence. */ UmiStatus umi_data_pool_health_compute(const UmiDataConnectionPool *pool,UmiDataPoolHealth *out_health);

#ifdef __cplusplus
}
#endif
#endif
