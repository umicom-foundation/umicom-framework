/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/api_baseline.h
 *
 * PURPOSE:
 *   Publish the public api baseline contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_API_BASELINE_H
#define UMICOM_CODEGUARD_API_BASELINE_H
#include "umicom/codeguard/quality_types.h"

/* A symbol record stores a stable name and a caller-calculated signature hash.
 * Deprecation permits a planned warning period before reviewed removal. */
typedef struct UmiCodeGuardApiSymbol {
    char name[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    uint64_t signature_hash;
    bool deprecated;
} UmiCodeGuardApiSymbol;

/* A baseline is a bounded, caller-owned snapshot for one public API release. */
typedef struct UmiCodeGuardApiBaseline {
    char name[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    unsigned major;
    unsigned minor;
    UmiCodeGuardApiSymbol symbols[UMI_CODEGUARD_QUALITY_MAX_SYMBOLS];
    size_t count;
} UmiCodeGuardApiBaseline;

/* One change explains compatibility impact for a named public symbol. */
typedef struct UmiCodeGuardApiChange {
    char symbol[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    UmiCodeGuardCompatibilityLevel level;
    char summary[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardApiChange;

/* Comparison totals support release gates while retaining individual evidence. */
typedef struct UmiCodeGuardApiComparison {
    UmiCodeGuardApiChange changes[UMI_CODEGUARD_QUALITY_MAX_CHANGES];
    size_t count;
    size_t additions;
    size_t removals;
    size_t signature_changes;
} UmiCodeGuardApiComparison;

/* Initialise empty caller-owned baseline storage for a non-zero major release. */
UmiStatus umi_codeguard_api_baseline_init(
    UmiCodeGuardApiBaseline *baseline,
    const char *name,
    unsigned major,
    unsigned minor);

/* Add one unique public symbol. The baseline copies the name and owns no pointer. */
UmiStatus umi_codeguard_api_baseline_add(
    UmiCodeGuardApiBaseline *baseline,
    const char *symbol,
    uint64_t signature_hash,
    bool deprecated);

/* Borrow a symbol record by exact name, or return NULL when it is absent. */
const UmiCodeGuardApiSymbol *umi_codeguard_api_baseline_find(
    const UmiCodeGuardApiBaseline *baseline,
    const char *symbol);

/* Compare expected and current snapshots, producing caller-owned change evidence. */
UmiStatus umi_codeguard_api_baseline_compare(
    const UmiCodeGuardApiBaseline *expected,
    const UmiCodeGuardApiBaseline *current,
    UmiCodeGuardApiComparison *out_comparison);

/* Return true when any recorded change requires an ABI/API compatibility plan. */
bool umi_codeguard_api_comparison_breaking(const UmiCodeGuardApiComparison *comparison);
#endif
