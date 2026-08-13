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

/* BEGINNER NOTE:
 * Remote feeds, folders and embedded catalogues can all populate this one
 * registry.  The registry stores bounded value snapshots owned by Framework.
 */
#ifndef UMICOM_DISTRIBUTION_REPOSITORY_H
#define UMICOM_DISTRIBUTION_REPOSITORY_H

#include "umicom/distribution/package.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDistributionRepository UmiDistributionRepository;

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

UmiStatus umi_distribution_repository_create(
    size_t capacity,
    UmiDistributionRepository **out_repository);
void umi_distribution_repository_destroy(UmiDistributionRepository *repository);
UmiStatus umi_distribution_repository_upsert(
    UmiDistributionRepository *repository,
    const UmiDistributionPackage *package);
UmiStatus umi_distribution_repository_find(
    const UmiDistributionRepository *repository,
    const char *release_id,
    UmiDistributionPackage *out_package);
UmiStatus umi_distribution_repository_at(
    const UmiDistributionRepository *repository,
    size_t index,
    UmiDistributionPackage *out_package);
size_t umi_distribution_repository_count(
    const UmiDistributionRepository *repository);
uint64_t umi_distribution_repository_revision(
    const UmiDistributionRepository *repository);
size_t umi_distribution_repository_query(
    const UmiDistributionRepository *repository,
    const UmiDistributionQuery *query,
    UmiDistributionPackage *out_packages,
    size_t capacity);
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
