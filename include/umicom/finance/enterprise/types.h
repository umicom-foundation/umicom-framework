/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/types.h
 *
 * PURPOSE:
 *   Define common enterprise valuation, market-data and portfolio-risk primitives.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_TYPES_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ENTERPRISE_ID_CAPACITY 64U
#define UMI_ENTERPRISE_NAME_CAPACITY 64U
#define UMI_ENTERPRISE_MAX_ITEMS 128U
#define UMI_ENTERPRISE_MAX_WORKERS 32U
#define UMI_ENTERPRISE_MAX_FACTORS 64U
#define UMI_ENTERPRISE_MAX_SCENARIOS 64U
#define UMI_ENTERPRISE_MAX_POSITIONS 128U
#define UMI_ENTERPRISE_MAX_HIERARCHY_NODES 64U

typedef enum UmiEnterpriseWorkState {
    UMI_ENTERPRISE_PENDING = 0,
    UMI_ENTERPRISE_RUNNING = 1,
    UMI_ENTERPRISE_COMPLETED = 2,
    UMI_ENTERPRISE_FAILED = 3,
    UMI_ENTERPRISE_CANCELLED = 4
} UmiEnterpriseWorkState;

typedef struct UmiEnterpriseNamedMetric {
    char name[UMI_ENTERPRISE_NAME_CAPACITY];
    double value;
} UmiEnterpriseNamedMetric;

/* Produce a deterministic FNV-1a fingerprint for bounded identifiers and cache keys. */
uint64_t umi_enterprise_hash_text(const char *text);
/* Initialise one named finite metric used by risk and service snapshots. */
UmiStatus umi_enterprise_metric_init(UmiEnterpriseNamedMetric *metric, const char *name, double value);

#ifdef __cplusplus
}
#endif

#endif
