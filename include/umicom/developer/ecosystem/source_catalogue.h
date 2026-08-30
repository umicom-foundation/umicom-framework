/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/source_catalogue.h
 * PURPOSE: Model remote, local and offline ecosystem package sources.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_SOURCE_CATALOGUE_H
#define UMICOM_DEVELOPER_ECOSYSTEM_SOURCE_CATALOGUE_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEcosystemSource {
    char source_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char display_name[UMI_ECOSYSTEM_NAME_CAPACITY];
    char endpoint[UMI_ECOSYSTEM_PATH_CAPACITY];
    char cache_path[UMI_ECOSYSTEM_PATH_CAPACITY];
    UmiEcosystemSourceKind kind;
    int priority;
    bool enabled;
    bool trusted;
    bool supports_offline;
    bool synchronising;
    UmiStatus last_status;
    uint64_t last_sync_ms;
    uint64_t revision;
} UmiEcosystemSource;

typedef struct UmiEcosystemSourceCatalogue {
    UmiEcosystemSource sources[UMI_ECOSYSTEM_MAX_SOURCES];
    size_t source_count;
    bool offline_mode;
    uint64_t revision;
} UmiEcosystemSourceCatalogue;

void umi_ecosystem_source_init(
    UmiEcosystemSource *source,
    const char *source_id,
    const char *display_name,
    UmiEcosystemSourceKind kind,
    const char *endpoint);
void umi_ecosystem_source_catalogue_init(
    UmiEcosystemSourceCatalogue *catalogue);
UmiStatus umi_ecosystem_source_catalogue_upsert(
    UmiEcosystemSourceCatalogue *catalogue,
    const UmiEcosystemSource *source);
UmiEcosystemSource *umi_ecosystem_source_catalogue_find(
    UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id);
const UmiEcosystemSource *umi_ecosystem_source_catalogue_find_const(
    const UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id);
UmiStatus umi_ecosystem_source_catalogue_set_offline(
    UmiEcosystemSourceCatalogue *catalogue,
    bool offline_mode);
UmiStatus umi_ecosystem_source_complete_sync(
    UmiEcosystemSourceCatalogue *catalogue,
    const char *source_id,
    UmiStatus status,
    uint64_t timestamp_ms);
bool umi_ecosystem_source_available(
    const UmiEcosystemSourceCatalogue *catalogue,
    const UmiEcosystemSource *source);
size_t umi_ecosystem_source_available_count(
    const UmiEcosystemSourceCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_SOURCE_CATALOGUE_H */
