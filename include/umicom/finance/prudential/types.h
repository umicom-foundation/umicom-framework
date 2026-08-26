/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/types.h
 *
 * PURPOSE:
 *   Define common prudential capital, liquidity and enterprise stress-testing primitives.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TYPES_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRU_ID_CAPACITY 64U
#define UMI_PRU_NAME_CAPACITY 96U
#define UMI_PRU_TEXT_CAPACITY 192U
#define UMI_PRU_MAX_ITEMS 128U
#define UMI_PRU_MAX_SCENARIOS 64U
#define UMI_PRU_MAX_PERIODS 64U

typedef enum UmiPrudentialSeverity {
    UMI_PRU_SEVERITY_INFO = 0,
    UMI_PRU_SEVERITY_WARNING = 1,
    UMI_PRU_SEVERITY_BREACH = 2,
    UMI_PRU_SEVERITY_CRITICAL = 3
} UmiPrudentialSeverity;

typedef enum UmiCapitalTierKind {
    UMI_PRU_CAPITAL_CET1 = 0,
    UMI_PRU_CAPITAL_AT1 = 1,
    UMI_PRU_CAPITAL_TIER2 = 2
} UmiCapitalTierKind;

typedef enum UmiHqlaLevel {
    UMI_PRU_HQLA_LEVEL1 = 0,
    UMI_PRU_HQLA_LEVEL2A = 1,
    UMI_PRU_HQLA_LEVEL2B = 2
} UmiHqlaLevel;

typedef enum UmiStressDirection {
    UMI_PRU_STRESS_DECREASE = -1,
    UMI_PRU_STRESS_INCREASE = 1
} UmiStressDirection;

typedef enum UmiStressRunState {
    UMI_PRU_STRESS_DRAFT = 0,
    UMI_PRU_STRESS_READY = 1,
    UMI_PRU_STRESS_RUNNING = 2,
    UMI_PRU_STRESS_COMPLETE = 3,
    UMI_PRU_STRESS_FAILED = 4
} UmiStressRunState;

typedef struct UmiPrudentialNamedMetric {
    char name[UMI_PRU_NAME_CAPACITY];
    double value;
} UmiPrudentialNamedMetric;

/* Copy bounded text and reject silent truncation at prudential control boundaries. */
UmiStatus umi_pru_copy_text(char *destination, size_t capacity, const char *source);
/* Generate a deterministic FNV-1a fingerprint for scenario, evidence and cache keys. */
uint64_t umi_pru_hash_text(const char *text);
/* Return one only for finite numeric inputs accepted by prudential calculations. */
int umi_pru_number_valid(double value);
/* Compute a guarded ratio and reject zero denominators or non-finite results. */
UmiStatus umi_pru_ratio(double numerator, double denominator, double *out_ratio);
/* Apply a non-negative scalar weight to an exposure or liquidity amount. */
UmiStatus umi_pru_weighted_amount(double amount, double weight, double *out_amount);

#ifdef __cplusplus
}
#endif

#endif
