/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/operations_contract.h
 *
 * PURPOSE:
 *   Publish the public operations contract contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * List the named operations trend values accepted by this public contract.
 */
typedef enum UmiOperationsTrend { UMI_OPERATIONS_TREND_UNKNOWN = 0, UMI_OPERATIONS_TREND_IMPROVED, UMI_OPERATIONS_TREND_STABLE, UMI_OPERATIONS_TREND_REGRESSED } UmiOperationsTrend;
/**
 * List the named operations health values accepted by this public contract.
 */
typedef enum UmiOperationsHealth { UMI_OPERATIONS_HEALTH_UNKNOWN = 0, UMI_OPERATIONS_HEALTH_READY, UMI_OPERATIONS_HEALTH_DEGRADED, UMI_OPERATIONS_HEALTH_FAILED } UmiOperationsHealth;
#endif
