/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/api_baseline.c
 *
 * PURPOSE:
 *   Store bounded public API snapshots and compare them so release tooling can
 *   explain additions, removals and incompatible signature changes safely.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/api_baseline.h"

#include <string.h>

/* Initialise a clean baseline and copy its release identity into owned storage. */
UmiStatus umi_codeguard_api_baseline_init(
    UmiCodeGuardApiBaseline *baseline,
    const char *name,
    unsigned major,
    unsigned minor)
{
    UmiStatus status;
    /* A zero major release cannot act as a stable compatibility baseline. */
    if (baseline == NULL || name == NULL || name[0] == '\0' || major == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(baseline, 0, sizeof(*baseline));
    status = umi_codeguard_quality_copy(
        baseline->name, sizeof(baseline->name), name);
    /* Do not publish release numbers when the baseline identity did not fit. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    baseline->major = major;
    baseline->minor = minor;
    return UMI_STATUS_OK;
}

/* Search exact names because public symbol spelling is case-sensitive ABI state. */
const UmiCodeGuardApiSymbol *umi_codeguard_api_baseline_find(
    const UmiCodeGuardApiBaseline *baseline,
    const char *symbol)
{
    size_t index;
    /* Invalid lookup input has no matching borrowed record. */
    if (baseline == NULL || symbol == NULL) {
        return NULL;
    }
    /* Return a borrowed pointer into caller-owned baseline storage. */
    for (index = 0U; index < baseline->count; ++index) {
        /* Exact equality identifies the one stable public symbol requested. */
        if (strcmp(baseline->symbols[index].name, symbol) == 0) {
            return &baseline->symbols[index];
        }
    }
    return NULL;
}

/* Copy one unique symbol only after validating identity and bounded capacity. */
UmiStatus umi_codeguard_api_baseline_add(
    UmiCodeGuardApiBaseline *baseline,
    const char *symbol,
    uint64_t signature_hash,
    bool deprecated)
{
    UmiStatus status;
    /* A zero hash cannot distinguish a deliberately calculated signature. */
    if (baseline == NULL || symbol == NULL || symbol[0] == '\0' ||
        signature_hash == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Duplicate names would make later compatibility comparison ambiguous. */
    if (umi_codeguard_api_baseline_find(baseline, symbol) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Fixed storage prevents allocation and lifetime surprises in release gates. */
    if (baseline->count >= UMI_CODEGUARD_QUALITY_MAX_SYMBOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_codeguard_quality_copy(
        baseline->symbols[baseline->count].name,
        sizeof(baseline->symbols[baseline->count].name), symbol);
    /* Publish the record only after its name has been copied successfully. */
    if (status == UMI_STATUS_OK) {
        baseline->symbols[baseline->count].signature_hash = signature_hash;
        baseline->symbols[baseline->count].deprecated = deprecated;
        baseline->count += 1U;
    }
    return status;
}

/* Append one bounded compatibility finding to caller-owned comparison evidence. */
static UmiStatus add_change(
    UmiCodeGuardApiComparison *comparison,
    const char *symbol,
    UmiCodeGuardCompatibilityLevel level,
    const char *summary)
{
    UmiCodeGuardApiChange *change;
    UmiStatus status;
    /* Stop before selecting a slot outside fixed comparison storage. */
    if (comparison->count >= UMI_CODEGUARD_QUALITY_MAX_CHANGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    change = &comparison->changes[comparison->count];
    (void)memset(change, 0, sizeof(*change));
    status = umi_codeguard_quality_copy(
        change->symbol, sizeof(change->symbol), symbol);
    /* A truncated symbol could point reviewers at the wrong public contract. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    change->level = level;
    status = umi_codeguard_quality_copy(
        change->summary, sizeof(change->summary), summary);
    /* Keep the slot unpublished when its human explanation did not fit. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    comparison->count += 1U;
    return UMI_STATUS_OK;
}

/* Compare removals/signature changes first, then record newly added symbols. */
UmiStatus umi_codeguard_api_baseline_compare(
    const UmiCodeGuardApiBaseline *expected,
    const UmiCodeGuardApiBaseline *current,
    UmiCodeGuardApiComparison *out_comparison)
{
    size_t index;
    UmiStatus status;
    /* All three objects are required because comparison writes complete evidence. */
    if (expected == NULL || current == NULL || out_comparison == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_comparison, 0, sizeof(*out_comparison));
    /* Every expected symbol must still exist with an unchanged signature. */
    for (index = 0U; index < expected->count; ++index) {
        const UmiCodeGuardApiSymbol *now = umi_codeguard_api_baseline_find(
            current, expected->symbols[index].name);
        /* Deprecated removals warn; removing a supported symbol is breaking. */
        if (now == NULL) {
            UmiCodeGuardCompatibilityLevel level =
                expected->symbols[index].deprecated
                ? UMI_CODEGUARD_COMPATIBILITY_WARNING
                : UMI_CODEGUARD_BREAKING;
            status = add_change(
                out_comparison,
                expected->symbols[index].name,
                level,
                "Public symbol removed from the current baseline");
            /* Preserve the exact storage or validation failure for diagnostics. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            out_comparison->removals += 1U;
        /* A retained name with a changed signature is still binary breaking. */
        } else if (now->signature_hash !=
                   expected->symbols[index].signature_hash) {
            status = add_change(
                out_comparison,
                expected->symbols[index].name,
                UMI_CODEGUARD_BREAKING,
                "Public symbol signature changed");
            /* Stop if complete evidence could not be stored safely. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            out_comparison->signature_changes += 1U;
        }
    }
    /* New current symbols are compatible additions unless another policy says otherwise. */
    for (index = 0U; index < current->count; ++index) {
        /* Absence from the expected baseline proves this symbol is newly published. */
        if (umi_codeguard_api_baseline_find(
                expected, current->symbols[index].name) == NULL) {
            status = add_change(
                out_comparison,
                current->symbols[index].name,
                UMI_CODEGUARD_COMPATIBLE,
                "Public symbol added");
            /* Stop if the compatible addition could not be recorded completely. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            out_comparison->additions += 1U;
        }
    }
    return UMI_STATUS_OK;
}

/* Fail closed for missing evidence and report true on the first breaking change. */
bool umi_codeguard_api_comparison_breaking(const UmiCodeGuardApiComparison *comparison)
{
    size_t index;
    /* A missing comparison cannot prove compatibility and therefore blocks release. */
    if (comparison == NULL) {
        return true;
    }
    /* One breaking item is enough; callers can still inspect the full comparison. */
    for (index = 0U; index < comparison->count; ++index) {
        /* Stop only for the explicit breaking classification. */
        if (comparison->changes[index].level == UMI_CODEGUARD_BREAKING) {
            return true;
        }
    }
    return false;
}
