/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/language_matrix.h
 *
 * PURPOSE:
 *   Model the languages, source roots, standards and interoperability policy
 *   of a polyglot project without coupling the project system to a compiler or
 *   user-interface toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_LANGUAGE_MATRIX_H
#define UMICOM_PROJECT_LANGUAGE_MATRIX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_LANGUAGE_MATRIX_API_VERSION 1U
#define UMI_PROJECT_LANGUAGE_CAPACITY 32U
#define UMI_PROJECT_LANGUAGE_ID_CAPACITY 64U
#define UMI_PROJECT_LANGUAGE_PATHS_CAPACITY 1024U

/**
 * List the named project language interop policy values accepted by this public contract.
 */
typedef enum UmiProjectLanguageInteropPolicy {
    UMI_PROJECT_LANGUAGE_INTEROP_NONE = 0,
    UMI_PROJECT_LANGUAGE_INTEROP_C_ABI = 1,
    UMI_PROJECT_LANGUAGE_INTEROP_TRANSPILE_TO_C = 2,
    UMI_PROJECT_LANGUAGE_INTEROP_FOREIGN_RUNTIME = 3,
    UMI_PROJECT_LANGUAGE_INTEROP_DATA_EXCHANGE = 4
} UmiProjectLanguageInteropPolicy;

/**
 * Represent the project language entry snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectLanguageEntrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char language_id[UMI_PROJECT_LANGUAGE_ID_CAPACITY];
    char source_roots[UMI_PROJECT_LANGUAGE_PATHS_CAPACITY];
    char generated_roots[UMI_PROJECT_LANGUAGE_PATHS_CAPACITY];
    char standard[128];
    UmiProjectLanguageInteropPolicy interop_policy;
    uint32_t compile_order;
    int enabled;
    uint64_t revision;
} UmiProjectLanguageEntrySnapshot;

/**
 * Represent the project language matrix snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectLanguageMatrixSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char project_id[128];
    char primary_language_id[UMI_PROJECT_LANGUAGE_ID_CAPACITY];
    size_t language_count;
    size_t enabled_count;
    size_t c_abi_count;
    size_t transpile_to_c_count;
    uint64_t revision;
} UmiProjectLanguageMatrixSnapshot;

/**
 * Represent the project language validation report data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectLanguageValidationReport {
    uint32_t struct_size;
    uint32_t api_version;
    size_t error_count;
    size_t warning_count;
    int has_primary_language;
    int primary_language_enabled;
    int compile_order_is_unique;
    char summary[512];
} UmiProjectLanguageValidationReport;

/**
 * Represent the project language matrix data shared with callers of this public contract.
 */
typedef struct UmiProjectLanguageMatrix UmiProjectLanguageMatrix;

/**
 * Initialise project language entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_project_language_entry_init(UmiProjectLanguageEntrySnapshot *entry);
/**
 * Initialise project language matrix from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_language_matrix_create(
    const char *project_id,
    UmiProjectLanguageMatrix **out_matrix);
/**
 * Release or reset state held by project language matrix so the same storage can be reused
 * safely.
 */
void umi_project_language_matrix_destroy(UmiProjectLanguageMatrix *matrix);
/**
 * Provide the project language matrix set primary operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_language_matrix_set_primary(
    UmiProjectLanguageMatrix *matrix,
    const char *language_id);
/**
 * Provide the project language matrix upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_language_matrix_upsert(
    UmiProjectLanguageMatrix *matrix,
    const UmiProjectLanguageEntrySnapshot *entry);
/**
 * Remove project language matrix while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_language_matrix_remove(
    UmiProjectLanguageMatrix *matrix,
    const char *language_id);
/**
 * Find project language matrix while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_language_matrix_find(
    const UmiProjectLanguageMatrix *matrix,
    const char *language_id,
    UmiProjectLanguageEntrySnapshot *out_entry);
/**
 * Find project language matrix while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_language_matrix_at(
    const UmiProjectLanguageMatrix *matrix,
    size_t index,
    UmiProjectLanguageEntrySnapshot *out_entry);
/**
 * Provide the project language matrix snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_language_matrix_snapshot(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageMatrixSnapshot *out_snapshot);
/**
 * Check that project language matrix satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_project_language_matrix_validate(
    const UmiProjectLanguageMatrix *matrix,
    UmiProjectLanguageValidationReport *out_report);
/**
 * Return the number of records represented by project language matrix without changing
 * their state.
 */
size_t umi_project_language_matrix_count(
    const UmiProjectLanguageMatrix *matrix);
/**
 * Provide the project language interop policy text operation used by this module and its
 * client applications.
 */
const char *umi_project_language_interop_policy_text(
    UmiProjectLanguageInteropPolicy policy);

#ifdef __cplusplus
}
#endif

#endif
