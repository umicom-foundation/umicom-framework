/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/repository.h
 *
 * PURPOSE:
 *   Maintain provider-neutral release repositories and deterministic local
 *   marketplace queries without performing network access.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/*
 * Remote feeds, folders and embedded catalogues can all populate this one
 * registry.  The registry stores bounded value snapshots owned by Framework.
 */
#ifndef UMICOM_DISTRIBUTION_REPOSITORY_H
#define UMICOM_DISTRIBUTION_REPOSITORY_H

#include "umicom/distribution/package.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the distribution repository data shared with callers of this public contract.
 */
typedef struct UmiDistributionRepository UmiDistributionRepository;

/**
 * Represent the distribution query data shared with callers of this public contract.
 */
typedef struct UmiDistributionQuery {
    const char *text;
    UmiDistributionPackageKind kind;
    UmiReleaseChannel channel;
    uint32_t framework_abi;
    int trusted_only;
    int compatible_only;
    int security_only;
    int include_deprecated;
} UmiDistributionQuery;

/**
 * Initialise distribution repository from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_distribution_repository_create(
    size_t capacity,
    UmiDistributionRepository **out_repository);
/**
 * Release or reset state held by distribution repository so the same storage can be reused
 * safely.
 */
void umi_distribution_repository_destroy(UmiDistributionRepository *repository);
/**
 * Provide the distribution repository upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_repository_upsert(
    UmiDistributionRepository *repository,
    const UmiDistributionPackage *package);
/**
 * Find distribution repository while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_distribution_repository_find(
    const UmiDistributionRepository *repository,
    const char *release_id,
    UmiDistributionPackage *out_package);
/**
 * Find distribution repository while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_distribution_repository_at(
    const UmiDistributionRepository *repository,
    size_t index,
    UmiDistributionPackage *out_package);
/**
 * Return the number of records represented by distribution repository without changing
 * their state.
 */
size_t umi_distribution_repository_count(
    const UmiDistributionRepository *repository);
/**
 * Provide the distribution repository revision operation used by this module and its
 * client applications.
 */
uint64_t umi_distribution_repository_revision(
    const UmiDistributionRepository *repository);
/**
 * Provide the distribution repository query operation used by this module and its client
 * applications.
 */
size_t umi_distribution_repository_query(
    const UmiDistributionRepository *repository,
    const UmiDistributionQuery *query,
    UmiDistributionPackage *out_packages,
    size_t capacity);
/**
 * Provide the distribution repository latest operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_repository_latest(
    const UmiDistributionRepository *repository,
    const char *product_id,
    UmiReleaseChannel maximum_channel,
    uint32_t framework_abi,
    UmiDistributionPackage *out_package);

#ifdef __cplusplus
}
#endif

#endif
