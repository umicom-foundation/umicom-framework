/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/evidence.c
 *
 * PURPOSE:
 *   Implement release-evidence storage and policy-oriented verification.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/evidence.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDistributionEvidenceRegistry {
    UmiDistributionEvidence items[UMI_DISTRIBUTION_EVIDENCE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDistributionEvidenceRegistry *registry,
                         const char *release_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || release_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].release_id, release_id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise distribution evidence registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_evidence_registry_create(
    UmiDistributionEvidenceRegistry **out_registry)
{
    UmiDistributionEvidenceRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDistributionEvidenceRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by distribution evidence registry so the same storage can be
 * reused safely.
 */
void umi_distribution_evidence_registry_destroy(
    UmiDistributionEvidenceRegistry *registry)
{
    free(registry);
}

/*
 * Provide the distribution evidence registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_distribution_evidence_registry_upsert(
    UmiDistributionEvidenceRegistry *registry,
    const UmiDistributionEvidence *evidence)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || evidence == NULL || evidence->release_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, evidence->release_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_DISTRIBUTION_EVIDENCE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    registry->items[index] = *evidence;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiDistributionEvidence);
    registry->items[index].api_version = UMI_DISTRIBUTION_API_VERSION;
    ++registry->revision;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Find distribution evidence registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_evidence_registry_find(
    const UmiDistributionEvidenceRegistry *registry,
    const char *release_id,
    UmiDistributionEvidence *out_evidence)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || release_id == NULL || out_evidence == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, release_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_evidence = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find distribution evidence registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_evidence_registry_at(
    const UmiDistributionEvidenceRegistry *registry,
    size_t index,
    UmiDistributionEvidence *out_evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_evidence = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by distribution evidence registry without
 * changing their state.
 */
size_t umi_distribution_evidence_registry_count(
    const UmiDistributionEvidenceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Check that distribution evidence satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_distribution_evidence_validate(
    const UmiDistributionEvidence *evidence,
    int require_signature,
    int require_sbom,
    int require_provenance,
    char *out_reason,
    size_t reason_capacity)
{
    const char *reason = "verified distribution evidence";
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL || out_reason == NULL || reason_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!evidence->checksum_verified || evidence->checksum[0] == '\0') {
        reason = "checksum evidence is missing or unverified";
        status = UMI_STATUS_PERMISSION_DENIED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (require_signature &&
               (!evidence->signature_verified || evidence->signature_id[0] == '\0')) {
        reason = "a verified package signature is required";
        status = UMI_STATUS_PERMISSION_DENIED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (require_sbom &&
               (!evidence->sbom_available || evidence->sbom_components == 0U ||
                evidence->licence_entries == 0U)) {
        reason = "SBOM and licence inventory evidence is required";
        status = UMI_STATUS_PERMISSION_DENIED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (require_provenance &&
               (!evidence->provenance_verified || evidence->source_revision[0] == '\0' ||
                evidence->builder_id[0] == '\0')) {
        reason = "verified source and builder provenance is required";
        status = UMI_STATUS_PERMISSION_DENIED;
    }
    (void)snprintf(out_reason, reason_capacity, "%s", reason);
    return status;
}
