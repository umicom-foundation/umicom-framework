/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/evidence.h
 *
 * PURPOSE:
 *   Correlate checksum, signature, SBOM, licence and build-provenance evidence
 *   with a marketplace release before installation is approved.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/*
 * Evidence records summarise verification results.  Cryptographic adapters and
 * SBOM parsers remain separate; policy consumes this stable result contract.
 */
#ifndef UMICOM_DISTRIBUTION_EVIDENCE_H
#define UMICOM_DISTRIBUTION_EVIDENCE_H

#include "umicom/distribution/package.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DISTRIBUTION_EVIDENCE_CAPACITY 1024U

/**
 * Represent the distribution evidence data shared with callers of this public contract.
 */
typedef struct UmiDistributionEvidence {
    uint32_t struct_size;
    uint32_t api_version;
    char release_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char checksum[UMI_DELIVERY_DIGEST_CAPACITY];
    char signature_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char source_revision[UMI_DISTRIBUTION_ID_CAPACITY];
    char builder_id[UMI_DISTRIBUTION_ID_CAPACITY];
    size_t sbom_components;
    size_t licence_entries;
    int checksum_verified;
    int signature_verified;
    int sbom_available;
    int provenance_verified;
    uint64_t verified_at_ms;
    uint64_t revision;
} UmiDistributionEvidence;

/**
 * Represent the distribution evidence registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDistributionEvidenceRegistry UmiDistributionEvidenceRegistry;

/**
 * Initialise distribution evidence registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_evidence_registry_create(
    UmiDistributionEvidenceRegistry **out_registry);
/**
 * Release or reset state held by distribution evidence registry so the same storage can be
 * reused safely.
 */
void umi_distribution_evidence_registry_destroy(
    UmiDistributionEvidenceRegistry *registry);
/**
 * Provide the distribution evidence registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_distribution_evidence_registry_upsert(
    UmiDistributionEvidenceRegistry *registry,
    const UmiDistributionEvidence *evidence);
/**
 * Find distribution evidence registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_evidence_registry_find(
    const UmiDistributionEvidenceRegistry *registry,
    const char *release_id,
    UmiDistributionEvidence *out_evidence);
/**
 * Find distribution evidence registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_evidence_registry_at(
    const UmiDistributionEvidenceRegistry *registry,
    size_t index,
    UmiDistributionEvidence *out_evidence);
/**
 * Return the number of records represented by distribution evidence registry without
 * changing their state.
 */
size_t umi_distribution_evidence_registry_count(
    const UmiDistributionEvidenceRegistry *registry);
/**
 * Check that distribution evidence satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_distribution_evidence_validate(
    const UmiDistributionEvidence *evidence,
    int require_signature,
    int require_sbom,
    int require_provenance,
    char *out_reason,
    size_t reason_capacity);

#ifdef __cplusplus
}
#endif

#endif
