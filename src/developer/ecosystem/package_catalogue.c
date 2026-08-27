/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/package_catalogue.c
 * PURPOSE: Implement the unified, provider-neutral ecosystem package projection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/package_catalogue.h"

#include <string.h>

void umi_ecosystem_package_record_init(
    UmiEcosystemPackageRecord *record,
    const char *package_id,
    const char *display_name,
    UmiEcosystemPackageKind kind,
    const char *source_id,
    const char *available_version)
{
    if (record == NULL) return;
    (void)memset(record, 0, sizeof(*record));
    if (package_id != NULL) {
        (void)umi_ecosystem_copy_text(
            record->package_id, sizeof(record->package_id), package_id);
    }
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(
            record->display_name, sizeof(record->display_name), display_name);
    }
    if (source_id != NULL) {
        (void)umi_ecosystem_copy_text(
            record->source_id, sizeof(record->source_id), source_id);
    }
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

UmiStatus umi_ecosystem_package_record_validate(
    const UmiEcosystemPackageRecord *record)
{
    if (record == NULL || record->package_id[0] == '\0' ||
        record->display_name[0] == '\0' || record->source_id[0] == '\0' ||
        record->available_version[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->kind < UMI_ECOSYSTEM_PACKAGE_EXTENSION ||
        record->kind > UMI_ECOSYSTEM_PACKAGE_TOOLCHAIN) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->installed && record->installed_version[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

bool umi_ecosystem_package_has_update(
    const UmiEcosystemPackageRecord *record)
{
    if (record == NULL || !record->installed ||
        record->installed_version[0] == '\0' ||
        record->available_version[0] == '\0') return false;
    return record->state == UMI_ECOSYSTEM_UPDATE_AVAILABLE ||
        strcmp(record->installed_version, record->available_version) != 0;
}

void umi_ecosystem_package_catalogue_init(
    UmiEcosystemPackageCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find(
    UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id)
{
    size_t index;
    if (catalogue == NULL || package_id == NULL) return NULL;
    for (index = 0U; index < catalogue->package_count; ++index) {
        if (strcmp(catalogue->packages[index].package_id, package_id) == 0) {
            return &catalogue->packages[index];
        }
    }
    return NULL;
}

const UmiEcosystemPackageRecord *umi_ecosystem_package_catalogue_find_const(
    const UmiEcosystemPackageCatalogue *catalogue,
    const char *package_id)
{
    size_t index;
    if (catalogue == NULL || package_id == NULL) return NULL;
    for (index = 0U; index < catalogue->package_count; ++index) {
        if (strcmp(catalogue->packages[index].package_id, package_id) == 0) {
            return &catalogue->packages[index];
        }
    }
    return NULL;
}

UmiStatus umi_ecosystem_package_catalogue_upsert(
    UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageRecord *record)
{
    UmiEcosystemPackageRecord *existing;
    UmiStatus status;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_package_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    existing = umi_ecosystem_package_catalogue_find(catalogue, record->package_id);
    if (existing != NULL) {
        *existing = *record;
        existing->revision++;
        catalogue->revision++;
        return UMI_STATUS_OK;
    }
    if (catalogue->package_count >= UMI_ECOSYSTEM_MAX_PACKAGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->packages[catalogue->package_count++] = *record;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_package_catalogue_statistics(
    const UmiEcosystemPackageCatalogue *catalogue,
    UmiEcosystemPackageStatistics *out_statistics)
{
    size_t index;
    if (catalogue == NULL || out_statistics == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_statistics, 0, sizeof(*out_statistics));
    out_statistics->total = catalogue->package_count;
    for (index = 0U; index < catalogue->package_count; ++index) {
        const UmiEcosystemPackageRecord *record = &catalogue->packages[index];
        if (record->installed) out_statistics->installed++;
        if (record->state == UMI_ECOSYSTEM_ACTIVE) out_statistics->active++;
        if (umi_ecosystem_package_has_update(record)) out_statistics->updates++;
        if (record->compatibility == UMI_ECOSYSTEM_INCOMPATIBLE) {
            out_statistics->incompatible++;
        }
        if (record->evidence != UMI_ECOSYSTEM_EVIDENCE_VERIFIED) {
            out_statistics->unverified++;
        }
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
