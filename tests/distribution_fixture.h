/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/distribution_fixture.h
 *
 * PURPOSE:
 *   Provide deterministic package and evidence fixtures shared by Product,
 *   Marketplace and Update Centre tests.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_TESTS_DISTRIBUTION_FIXTURE_H
#define UMICOM_TESTS_DISTRIBUTION_FIXTURE_H

#include <string.h>

#include "umicom/distribution/distribution.h"

static inline UmiDistributionPackage distribution_test_package(
    const char *release_id,
    const char *product_id,
    UmiVersion version,
    UmiReleaseChannel channel)
{
    UmiDistributionPackage package;
    (void)umi_distribution_package_init(
        &package, release_id, product_id, product_id, "Umicom Test Product",
        version, UMI_DISTRIBUTION_APPLICATION, channel);
    (void)strcpy(package.provider_id, "org.umicom.foundation");
    (void)strcpy(package.summary, "Verified distribution test package");
    (void)strcpy(package.category, "development");
    (void)strcpy(package.licence_id, "MIT");
    (void)strcpy(package.source_uri, "file:///catalogue/test.zip");
    package.required_framework_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    package.download_size = 4096U;
    package.trusted = 1;
    package.compatible = 1;
    return package;
}

static inline UmiDistributionEvidence distribution_test_evidence(
    const char *release_id)
{
    UmiDistributionEvidence evidence;
    (void)memset(&evidence, 0, sizeof(evidence));
    (void)strcpy(evidence.release_id, release_id);
    (void)strcpy(evidence.checksum, "sha256:test-digest");
    (void)strcpy(evidence.signature_id, "umicom-test-signature");
    (void)strcpy(evidence.source_revision, "0123456789abcdef");
    (void)strcpy(evidence.builder_id, "umicom-ci");
    evidence.sbom_components = 4U;
    evidence.licence_entries = 4U;
    evidence.checksum_verified = 1;
    evidence.signature_verified = 1;
    evidence.sbom_available = 1;
    evidence.provenance_verified = 1;
    evidence.verified_at_ms = 100U;
    return evidence;
}

#endif
