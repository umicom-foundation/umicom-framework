/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/types.h
 *
 * PURPOSE:
 *   Define common regulatory reporting, governance and control primitives.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_TYPES_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REG_ID_CAPACITY 64U
#define UMI_REG_NAME_CAPACITY 96U
#define UMI_REG_TEXT_CAPACITY 192U
#define UMI_REG_MAX_ITEMS 128U
#define UMI_REG_MAX_FIELDS 128U
#define UMI_REG_MAX_RULES 128U
#define UMI_REG_MAX_NODES 96U
#define UMI_REG_MAX_EDGES 192U
#define UMI_REG_MAX_HISTORY 128U
#define UMI_REG_MAX_JOBS 64U

/**
 * List the named reg severity values accepted by this public contract.
 */
typedef enum UmiRegSeverity {
    UMI_REG_SEVERITY_INFO = 0,
    UMI_REG_SEVERITY_WARNING = 1,
    UMI_REG_SEVERITY_ERROR = 2,
    UMI_REG_SEVERITY_CRITICAL = 3
} UmiRegSeverity;

/**
 * List the named reg lifecycle state values accepted by this public contract.
 */
typedef enum UmiRegLifecycleState {
    UMI_REG_STATE_DRAFT = 0,
    UMI_REG_STATE_VALIDATED = 1,
    UMI_REG_STATE_APPROVED = 2,
    UMI_REG_STATE_SUBMITTED = 3,
    UMI_REG_STATE_ACCEPTED = 4,
    UMI_REG_STATE_REJECTED = 5,
    UMI_REG_STATE_CANCELLED = 6
} UmiRegLifecycleState;

/**
 * Represent the reg named metric data shared with callers of this public contract.
 */
typedef struct UmiRegNamedMetric {
    char name[UMI_REG_NAME_CAPACITY];
    double value;
} UmiRegNamedMetric;

/* Copy bounded text and reject silent truncation at regulatory boundaries. */
UmiStatus umi_reg_copy_text(char *destination, size_t capacity, const char *source);
/* Generate a deterministic FNV-1a fingerprint for evidence and lineage keys. */
uint64_t umi_reg_hash_text(const char *text);
/* Return one only for finite numeric reporting values. */
int umi_reg_number_valid(double value);
/* Compute a guarded ratio used by capital, liquidity and quality metrics. */
UmiStatus umi_reg_ratio(double numerator, double denominator, double *out_ratio);

#ifdef __cplusplus
}
#endif

#endif
