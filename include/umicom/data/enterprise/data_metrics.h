/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_metrics.h
 *
 * PURPOSE:
 *   Capture bounded operational counters and derive useful rates for Data Server observability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_METRICS_H
#define UMICOM_DATA_ENTERPRISE_DATA_METRICS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data metrics data shared with callers of this public contract.
 */
typedef struct UmiDataMetrics { uint64_t queries; uint64_t query_failures; uint64_t transactions; uint64_t transaction_rollbacks; uint64_t pool_acquisitions; uint64_t pool_waits; uint64_t bytes_read; uint64_t bytes_written; double query_success_percent; double transaction_commit_percent; } UmiDataMetrics;
/* Recompute derived percentages from raw monotonic counters. */ UmiStatus umi_data_metrics_refresh(UmiDataMetrics *metrics);

#ifdef __cplusplus
}
#endif
#endif
