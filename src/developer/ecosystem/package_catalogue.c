/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/package_catalogue.c
 * PURPOSE: Implement the unified, provider-neutral ecosystem package projection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/package_catalogue.h"

#include <string.h>

/*
 * Initialise ecosystem package record from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_package_record_init(
    UmiEcosystemPackageRecord *record,
    const char *package_id,
    const char *display_name,
    UmiEcosystemPackageKind kind,
    const char *source_id,
    const char *available_version)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    (void)memset(record, 0, sizeof(*record));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package_id != NULL) {
        (void)umi_ecosystem_copy_text(
            record->package_id, sizeof(record->package_id), package_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(
            record->display_name, sizeof(record->display_name), display_name);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_id != NULL) {
        (void)umi_ecosystem_copy_text(
            record->source_id, sizeof(record->source_id), source_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (available_version != NULL) {
        (void)umi_ecosystem_copy_text(record->available_version,
            sizeof(record->available_version), available_version);
    }
    record->kind = kind;
    record->state = UMI_ECOSYSTEM_AVAILABLE;
    record->compatibility = UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN;
    record->evidence = UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
    record->enabled = true;
    record->revision = 1U;
}

/*
 * Check that ecosystem package record satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ecosystem_package_record_validate(
    const UmiEcosystemPackageRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->package_id[0] == '\0' ||
        record->display_name[0] == '\0' || record->source_id[0] == '\0' ||
        record->available_version[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->kind < UMI_ECOSYSTEM_PACKAGE_EXTENSION ||
        record->kind > UMI_ECOSYSTEM_PACKAGE_TOOLCHAIN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->installed && record->installed_version[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem package has update operation used by this module and its client
 * applications.
 */
bool umi_ecosystem_package_has_update(
    const UmiEcosystemPackageRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !record->installed ||
        record->installed_version[0] == '\0' ||
        record->available_version[0] == '\0') return false;
    return record->state == UMI_ECOSYSTEM_UPDATE_AVAILABLE ||
        strcmp(record->installed_version, record->available_version) != 0;
}

/*
 * Initialise ecosystem package catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_package_catalogue_init(
    UmiEcosystemPackageCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

/*
 * Find ecosystem package catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find(
    UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || package_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->package_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->packages[index].package_id, package_id) == 0) {
            return &catalogue->packages[index];
        }
    }
    return NULL;
}

/*
 * Provide the ecosystem package catalogue find const operation used by this module and its
 * client applications.
 */
const UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find_const(
    const UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || package_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->package_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->packages[index].package_id, package_id) == 0) {
            return &catalogue->packages[index];
        }
    }
    return NULL;
}

/*
 * Provide the ecosystem package catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_package_catalogue_upsert(
    UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageRecord *record)
{
    UmiEcosystemPackageRecord *existing;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_package_record_validate(record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    existing = umi_ecosystem_package_catalogue_find(catalogue, record->package_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) {
        *existing = *record;
        existing->revision++;
        catalogue->revision++;
        return UMI_STATUS_OK;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->package_count >= UMI_ECOSYSTEM_MAX_PACKAGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->packages[catalogue->package_count++] = *record;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem package catalogue statistics operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_package_catalogue_statistics(
    const UmiEcosystemPackageCatalogue *catalogue,
    UmiEcosystemPackageStatistics *out_statistics)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_statistics == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_statistics, 0, sizeof(*out_statistics));
    out_statistics->total = catalogue->package_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->package_count; ++index) {
        const UmiEcosystemPackageRecord *record = &catalogue->packages[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (record->installed) out_statistics->installed++;
        /* Apply this operation only while the related capability or state is available. */
        if (record->state == UMI_ECOSYSTEM_ACTIVE) out_statistics->active++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ecosystem_package_has_update(record)) out_statistics->updates++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (record->compatibility == UMI_ECOSYSTEM_INCOMPATIBLE) {
            out_statistics->incompatible++;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (record->evidence != UMI_ECOSYSTEM_EVIDENCE_VERIFIED) {
            out_statistics->unverified++;
        }
        /* Select the behaviour associated with the requested command or state value. */
        switch (record->kind) {
            case UMI_ECOSYSTEM_PACKAGE_EXTENSION: out_statistics->extensions++; break;
            case UMI_ECOSYSTEM_PACKAGE_SDK: out_statistics->sdks++; break;
            case UMI_ECOSYSTEM_PACKAGE_COMPONENT: out_statistics->components++; break;
            case UMI_ECOSYSTEM_PACKAGE_TEMPLATE: out_statistics->templates++; break;
            case UMI_ECOSYSTEM_PACKAGE_TOOLCHAIN: break;
            default: break;
        }
    }
    return UMI_STATUS_OK;
}
