/*-----------------------------------------------------------------------------
 * Umicom Framework | Operations Centre shared contracts
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_OPERATIONS_CONTRACT_H
#define UMICOM_OBSERVABILITY_OPERATIONS_CONTRACT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_OPERATIONS_ID_CAPACITY 128U
#define UMI_OPERATIONS_NAME_CAPACITY 192U
#define UMI_OPERATIONS_TEXT_CAPACITY 384U
#define UMI_OPERATIONS_MAX_RESOURCE_SAMPLES 512U
#define UMI_OPERATIONS_MAX_BENCHMARKS 256U
#define UMI_OPERATIONS_MAX_CRASHES 128U
typedef enum UmiOperationsTrend { UMI_OPERATIONS_TREND_UNKNOWN = 0, UMI_OPERATIONS_TREND_IMPROVED, UMI_OPERATIONS_TREND_STABLE, UMI_OPERATIONS_TREND_REGRESSED } UmiOperationsTrend;
typedef enum UmiOperationsHealth { UMI_OPERATIONS_HEALTH_UNKNOWN = 0, UMI_OPERATIONS_HEALTH_READY, UMI_OPERATIONS_HEALTH_DEGRADED, UMI_OPERATIONS_HEALTH_FAILED } UmiOperationsHealth;
#endif
