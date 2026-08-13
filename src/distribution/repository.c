/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/repository.c
 *
 * PURPOSE:
 *   Implement a bounded local marketplace repository with deterministic query
 *   and latest-compatible-release selection.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/repository.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiDistributionRepository {
    UmiDistributionPackage *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static size_t find_index(const UmiDistributionRepository *repository,
                         const char *release_id)
{
    size_t index;
    if (repository == NULL || release_id == NULL) return SIZE_MAX;
    for (index = 0U; index < repository->count; ++index) {
        if (strcmp(repository->items[index].release_id, release_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int text_contains(const char *haystack, const char *needle)
{
    size_t i;
    size_t j;
    if (needle == NULL || needle[0] == '\0') return 1;
    if (haystack == NULL) return 0;
    for (i = 0U; haystack[i] != '\0'; ++i) {
        for (j = 0U; needle[j] != '\0' && haystack[i + j] != '\0'; ++j) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j])) break;
        }
        if (needle[j] == '\0') return 1;
    }
    return 0;
}

static int channel_allowed(UmiReleaseChannel candidate,
                           UmiReleaseChannel requested)
{
    if (requested == UMI_RELEASE_STABLE) return candidate == UMI_RELEASE_STABLE;
    if (requested == UMI_RELEASE_BETA) {
        return candidate == UMI_RELEASE_BETA || candidate == UMI_RELEASE_STABLE;
    }
    if (requested == UMI_RELEASE_NIGHTLY) {
        return candidate == UMI_RELEASE_NIGHTLY || candidate == UMI_RELEASE_BETA ||
               candidate == UMI_RELEASE_STABLE;
    }
    return candidate >= UMI_RELEASE_DEVELOPMENT && candidate <= UMI_RELEASE_STABLE;
}

UmiStatus umi_distribution_repository_create(
    size_t capacity,
    UmiDistributionRepository **out_repository)
{
    UmiDistributionRepository *repository;
    if (out_repository == NULL || capacity == 0U ||
        capacity > UMI_DISTRIBUTION_PACKAGE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_repository = NULL;
    repository = (UmiDistributionRepository *)calloc(1U, sizeof(*repository));
    if (repository == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    repository->items = (UmiDistributionPackage *)calloc(
        capacity, sizeof(repository->items[0]));
    if (repository->items == NULL) {
        free(repository);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    repository->capacity = capacity;
    repository->revision = 1U;
    *out_repository = repository;
    return UMI_STATUS_OK;
}

void umi_distribution_repository_destroy(UmiDistributionRepository *repository)
{
    if (repository == NULL) return;
    free(repository->items);
    free(repository);
}

UmiStatus umi_distribution_repository_upsert(
    UmiDistributionRepository *repository,
    const UmiDistributionPackage *package)
{
    char reason[256];
    size_t index;
    UmiStatus status;
    if (repository == NULL || package == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_distribution_package_validate(package, reason, sizeof(reason));
    if (status != UMI_STATUS_OK) return status;
    index = find_index(repository, package->release_id);
    if (index == SIZE_MAX) {
        if (repository->count >= repository->capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = repository->count++;
    }
    repository->items[index] = *package;
    ++repository->revision;
    repository->items[index].revision = repository->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_repository_find(
    const UmiDistributionRepository *repository,
    const char *release_id,
    UmiDistributionPackage *out_package)
{
    size_t index;
    if (repository == NULL || release_id == NULL || out_package == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(repository, release_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_package = repository->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_repository_at(
    const UmiDistributionRepository *repository,
    size_t index,
    UmiDistributionPackage *out_package)
{
    if (repository == NULL || out_package == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= repository->count) return UMI_STATUS_NOT_FOUND;
    *out_package = repository->items[index];
    return UMI_STATUS_OK;
}

size_t umi_distribution_repository_count(const UmiDistributionRepository *repository)
{
    return repository != NULL ? repository->count : 0U;
}

uint64_t umi_distribution_repository_revision(const UmiDistributionRepository *repository)
{
    return repository != NULL ? repository->revision : 0U;
}

static int query_matches(const UmiDistributionPackage *package,
                         const UmiDistributionQuery *query)
{
    if (query == NULL) return 1;
    if (query->kind != 0 && package->kind != query->kind) return 0;
    if (query->channel != 0 && package->channel != query->channel) return 0;
    if (query->trusted_only && !package->trusted) return 0;
    if (query->compatible_only && !package->compatible) return 0;
    if (query->security_only && !package->security_update) return 0;
    if (!query->include_deprecated && package->deprecated) return 0;
    if (query->framework_abi != 0U &&
        package->required_framework_abi > query->framework_abi) return 0;
    if (!text_contains(package->display_name, query->text) &&
        !text_contains(package->summary, query->text) &&
        !text_contains(package->product_id, query->text) &&
        !text_contains(package->category, query->text)) return 0;
    return 1;
}

size_t umi_distribution_repository_query(
    const UmiDistributionRepository *repository,
    const UmiDistributionQuery *query,
    UmiDistributionPackage *out_packages,
    size_t capacity)
{
    size_t index;
    size_t written = 0U;
    if (repository == NULL || (capacity > 0U && out_packages == NULL)) return 0U;
    for (index = 0U; index < repository->count && written < capacity; ++index) {
        if (query_matches(&repository->items[index], query)) {
            out_packages[written++] = repository->items[index];
        }
    }
    return written;
}

UmiStatus umi_distribution_repository_latest(
    const UmiDistributionRepository *repository,
    const char *product_id,
    UmiReleaseChannel maximum_channel,
    uint32_t framework_abi,
    UmiDistributionPackage *out_package)
{
    size_t index;
    size_t best = SIZE_MAX;
    if (repository == NULL || product_id == NULL || out_package == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < repository->count; ++index) {
        const UmiDistributionPackage *candidate = &repository->items[index];
        int comparison;
        if (strcmp(candidate->product_id, product_id) != 0 ||
            candidate->deprecated || !candidate->compatible ||
            candidate->required_framework_abi > framework_abi ||
            !channel_allowed(candidate->channel, maximum_channel)) continue;
        if (best == SIZE_MAX) {
            best = index;
            continue;
        }
        comparison = umi_distribution_version_compare(
            candidate->version, repository->items[best].version);
        if (comparison > 0 ||
            (comparison == 0 && candidate->channel > repository->items[best].channel)) {
            best = index;
        }
    }
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_package = repository->items[best];
    return UMI_STATUS_OK;
}
