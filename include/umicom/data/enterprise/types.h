/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/types.h
 *
 * PURPOSE:
 *   Define stable C23 types, capacities, states, value kinds and utility helpers shared by the enterprise Data Server platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_TYPES_H
#define UMICOM_DATA_ENTERPRISE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DATA_ENTERPRISE_ID_CAPACITY 128U
#define UMI_DATA_ENTERPRISE_TEXT_CAPACITY 256U
#define UMI_DATA_ENTERPRISE_PATH_CAPACITY 512U
#define UMI_DATA_ENTERPRISE_MAX_ITEMS 64U
#define UMI_DATA_ENTERPRISE_MAX_COLUMNS 64U
#define UMI_DATA_ENTERPRISE_MAX_EDGES 128U
#define UMI_DATA_ENTERPRISE_MAX_OPERATIONS 128U

/**
 * List the named data enterprise state values accepted by this public contract.
 */
typedef enum UmiDataEnterpriseState {
    UMI_DATA_ENTERPRISE_UNKNOWN = 0,
    UMI_DATA_ENTERPRISE_READY = 1,
    UMI_DATA_ENTERPRISE_DEGRADED = 2,
    UMI_DATA_ENTERPRISE_BLOCKED = 3,
    UMI_DATA_ENTERPRISE_FAILED = 4
} UmiDataEnterpriseState;

/**
 * List the named data value kind values accepted by this public contract.
 */
typedef enum UmiDataValueKind {
    UMI_DATA_VALUE_NULL = 0,
    UMI_DATA_VALUE_INTEGER = 1,
    UMI_DATA_VALUE_REAL = 2,
    UMI_DATA_VALUE_TEXT = 3,
    UMI_DATA_VALUE_BLOB = 4,
    UMI_DATA_VALUE_BOOLEAN = 5,
    UMI_DATA_VALUE_TIMESTAMP = 6,
    UMI_DATA_VALUE_DECIMAL = 7
} UmiDataValueKind;

/**
 * List the named data change kind values accepted by this public contract.
 */
typedef enum UmiDataChangeKind {
    UMI_DATA_CHANGE_NONE = 0,
    UMI_DATA_CHANGE_INSERT = 1,
    UMI_DATA_CHANGE_UPDATE = 2,
    UMI_DATA_CHANGE_DELETE = 3
} UmiDataChangeKind;

/**
 * List the named data compatibility values accepted by this public contract.
 */
typedef enum UmiDataCompatibility {
    UMI_DATA_COMPATIBLE = 1,
    UMI_DATA_COMPATIBLE_WITH_REBUILD = 2,
    UMI_DATA_BREAKING = 3
} UmiDataCompatibility;

/**
 * List the named data consistency values accepted by this public contract.
 */
typedef enum UmiDataConsistency {
    UMI_DATA_CONSISTENCY_EVENTUAL = 1,
    UMI_DATA_CONSISTENCY_SESSION = 2,
    UMI_DATA_CONSISTENCY_STRONG = 3
} UmiDataConsistency;

/* Copy text into a bounded Framework buffer and always terminate it. */
UmiStatus umi_data_enterprise_copy_text(char *destination, size_t capacity, const char *source);
/* Produce a deterministic 64-bit FNV-1a fingerprint for persistence and comparison evidence. */
uint64_t umi_data_enterprise_hash64(const void *data, size_t size, uint64_t seed);
/* Convert a numerator/denominator pair into a bounded percentage. */
double umi_data_enterprise_percent(uint64_t numerator, uint64_t denominator);
/* Return a stable human-readable state label. */
const char *umi_data_enterprise_state_text(UmiDataEnterpriseState state);

#ifdef __cplusplus
}
#endif
#endif
