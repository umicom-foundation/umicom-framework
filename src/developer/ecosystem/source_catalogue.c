/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/source_catalogue.c
 * PURPOSE: Implement deterministic source registration and offline availability.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/source_catalogue.h"

#include <string.h>

void umi_ecosystem_source_init(
    UmiEcosystemSource *source,
    const char *source_id,
    const char *display_name,
    UmiEcosystemSourceKind kind,
    const char *endpoint)
{
    if (source == NULL) return;
    (void)memset(source, 0, sizeof(*source));
    if (source_id != NULL) {
        (void)umi_ecosystem_copy_text(
            source->source_id, sizeof(source->source_id), source_id);
    }
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(
            source->display_name, sizeof(source->display_name), display_name);
    }
    if (endpoint != NULL) {
        (void)umi_ecosystem_copy_text(
            source->endpoint, sizeof(source->endpoint), endpoint);
    }
    source->kind = kind;
    source->enabled = true;
    source->last_status = UMI_STATUS_OK;
    source->revision = 1U;
}

void umi_ecosystem_source_catalogue_init(
    UmiEcosystemSourceCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

UmiEcosystemSource *umi_ecosystem_source_catalogue_find(
    UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id)
{
    size_t index;
    if (catalogue == NULL || source_id == NULL) return NULL;
    for (index = 0U; index < catalogue->source_count; ++index) {
        if (strcmp(catalogue->sources[index].source_id, source_id) == 0) {
            return &catalogue->sources[index];
        }
    }
    return NULL;
}

const UmiEcosystemSource *umi_ecosystem_source_catalogue_find_const(
    const UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id)
{
    size_t index;
    if (catalogue == NULL || source_id == NULL) return NULL;
    for (index = 0U; index < catalogue->source_count; ++index) {
        if (strcmp(catalogue->sources[index].source_id, source_id) == 0) {
            return &catalogue->sources[index];
        }
    }
    return NULL;
}

UmiStatus umi_ecosystem_source_catalogue_upsert(
    UmiEcosystemSourceCatalogue *catalogue,
    const UmiEcosystemSource *source)
{
    UmiEcosystemSource *existing;
    if (catalogue == NULL || source == NULL || source->source_id[0] == '\0' ||
        source->display_name[0] == '\0' || source->endpoint[0] == '\0' ||
        source->kind < UMI_ECOSYSTEM_SOURCE_REMOTE ||
        source->kind > UMI_ECOSYSTEM_SOURCE_OFFLINE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = umi_ecosystem_source_catalogue_find(catalogue, source->source_id);
    if (existing != NULL) {
        *existing = *source;
        existing->revision++;
        catalogue->revision++;
        return UMI_STATUS_OK;
    }
    if (catalogue->source_count >= UMI_ECOSYSTEM_MAX_SOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->sources[catalogue->source_count++] = *source;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_source_catalogue_set_offline(
    UmiEcosystemSourceCatalogue *catalogue,
    bool offline_mode)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->offline_mode != offline_mode) {
        catalogue->offline_mode = offline_mode;
        catalogue->revision++;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_source_complete_sync(
    UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id,
    UmiStatus status,
    uint64_t timestamp_ms)
{
    UmiEcosystemSource *source =
        umi_ecosystem_source_catalogue_find(catalogue, source_id);
    if (source == NULL) return UMI_STATUS_NOT_FOUND;
    source->synchronising = false;
    source->last_status = status;
    source->last_sync_ms = timestamp_ms;
    source->revision++;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

bool umi_ecosystem_source_available(
    const UmiEcosystemSourceCatalogue *catalogue,
    const UmiEcosystemSource *source)
{
    if (catalogue == NULL || source == NULL || !source->enabled) return false;
    if (!catalogue->offline_mode) return true;
    if (source->kind == UMI_ECOSYSTEM_SOURCE_LOCAL ||
        source->kind == UMI_ECOSYSTEM_SOURCE_OFFLINE) return true;
    return source->supports_offline && source->cache_path[0] != '\0';
}

size_t umi_ecosystem_source_available_count(
    const UmiEcosystemSourceCatalogue *catalogue)
{
    size_t count = 0U;
    size_t index;
    if (catalogue == NULL) return 0U;
    for (index = 0U; index < catalogue->source_count; ++index) {
        if (umi_ecosystem_source_available(catalogue, &catalogue->sources[index])) {
            ++count;
        }
    }
    return count;
}
