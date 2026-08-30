/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/package_query.c
 * PURPOSE: Implement deterministic, instance-local ecosystem package queries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/package_query.h"

#include <string.h>

void umi_ecosystem_package_query_init(UmiEcosystemPackageQuery *query)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->package_kind = -1;
    query->result_limit = UMI_ECOSYSTEM_MAX_RESULTS;
}

static bool source_matches(
    const UmiEcosystemSourceCatalogue *sources,
    const UmiEcosystemPackageQuery *query,
    const UmiEcosystemPackageRecord *record)
{
    const UmiEcosystemSource *source;
    if (query->source_id[0] != '\0' &&
        strcmp(query->source_id, record->source_id) != 0) return false;
    if (!query->available_sources_only) return true;
    if (sources == NULL) return false;
    source = umi_ecosystem_source_catalogue_find_const(sources, record->source_id);
    return source != NULL && umi_ecosystem_source_available(sources, source);
}

static bool package_matches(
    const UmiEcosystemSourceCatalogue *sources,
    const UmiEcosystemPackageQuery *query,
    const UmiEcosystemPackageRecord *record)
{
    if (query->package_kind >= 0 &&
        (int)record->kind != query->package_kind) return false;
    if (query->installed_only && !record->installed) return false;
    if (query->updates_only && !umi_ecosystem_package_has_update(record)) return false;
    if (query->verified_only &&
        record->evidence != UMI_ECOSYSTEM_EVIDENCE_VERIFIED) return false;
    if (query->compatible_only &&
        record->compatibility != UMI_ECOSYSTEM_COMPATIBLE &&
        record->compatibility != UMI_ECOSYSTEM_REQUIRES_MIGRATION) return false;
    if (!source_matches(sources, query, record)) return false;
    if (query->text[0] == '\0') return true;
    return umi_ecosystem_text_contains(record->package_id, query->text) ||
        umi_ecosystem_text_contains(record->display_name, query->text) ||
        umi_ecosystem_text_contains(record->publisher, query->text) ||
        umi_ecosystem_text_contains(record->description, query->text);
}

static uint32_t package_score(
    const UmiEcosystemPackageQuery *query,
    const UmiEcosystemPackageRecord *record)
{
    uint32_t score = 100U;
    if (query->text[0] != '\0') {
        score = 0U;
        if (strcmp(record->package_id, query->text) == 0) score += 1200U;
        else if (umi_ecosystem_text_contains(record->package_id, query->text)) {
            score += 900U;
        }
        if (umi_ecosystem_text_contains(record->display_name, query->text)) {
            score += 700U;
        }
        if (umi_ecosystem_text_contains(record->publisher, query->text)) {
            score += 400U;
        }
        if (umi_ecosystem_text_contains(record->description, query->text)) {
            score += 200U;
        }
    }
    if (record->installed) score += 50U;
    if (umi_ecosystem_package_has_update(record)) score += 80U;
    if (record->evidence == UMI_ECOSYSTEM_EVIDENCE_VERIFIED) score += 40U;
    if (record->compatibility == UMI_ECOSYSTEM_COMPATIBLE) score += 30U;
    score += record->rating_milli / 100U;
    return score;
}

static bool ranks_before(
    const UmiEcosystemPackageCatalogue *catalogue,
    size_t left_index,
    uint32_t left_score,
    size_t right_index,
    uint32_t right_score)
{
    if (left_score != right_score) return left_score > right_score;
    return strcmp(catalogue->packages[left_index].display_name,
        catalogue->packages[right_index].display_name) < 0;
}

static void insert_result(
    const UmiEcosystemPackageCatalogue *catalogue,
    UmiEcosystemPackageQueryResult *result,
    size_t package_index,
    uint32_t score,
    size_t limit)
{
    size_t position;
    if (limit == 0U) return;
    if (result->result_count < limit) {
        position = result->result_count++;
    } else {
        position = limit - 1U;
        if (!ranks_before(catalogue, package_index, score,
            result->package_indexes[position], result->scores[position])) return;
    }
    result->package_indexes[position] = package_index;
    result->scores[position] = score;
    while (position > 0U && ranks_before(catalogue,
        result->package_indexes[position], result->scores[position],
        result->package_indexes[position - 1U], result->scores[position - 1U])) {
        size_t swap_index = result->package_indexes[position - 1U];
        uint32_t swap_score = result->scores[position - 1U];
        result->package_indexes[position - 1U] = result->package_indexes[position];
        result->scores[position - 1U] = result->scores[position];
        result->package_indexes[position] = swap_index;
        result->scores[position] = swap_score;
        --position;
    }
}

UmiStatus umi_ecosystem_package_query_execute(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemSourceCatalogue *sources,
    const UmiEcosystemPackageQuery *query,
    UmiEcosystemPackageQueryResult *out_result)
{
    size_t index;
    size_t limit;
    if (catalogue == NULL || query == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_result, 0, sizeof(*out_result));
    limit = query->result_limit;
    if (limit > UMI_ECOSYSTEM_MAX_RESULTS) limit = UMI_ECOSYSTEM_MAX_RESULTS;
    for (index = 0U; index < catalogue->package_count; ++index) {
        const UmiEcosystemPackageRecord *record = &catalogue->packages[index];
        if (package_matches(sources, query, record)) {
            out_result->total_matches++;
            insert_result(catalogue, out_result, index,
                package_score(query, record), limit);
        }
    }
    out_result->catalogue_revision = catalogue->revision;
    out_result->source_revision = sources != NULL ? sources->revision : 0U;
    return UMI_STATUS_OK;
}

const UmiEcosystemPackageRecord *umi_ecosystem_package_query_result_at(
    const UmiEcosystemPackageCatalogue *catalogue,
    const UmiEcosystemPackageQueryResult *result,
    size_t index)
{
    size_t package_index;
    if (catalogue == NULL || result == NULL || index >= result->result_count) {
        return NULL;
    }
    package_index = result->package_indexes[index];
    return package_index < catalogue->package_count
        ? &catalogue->packages[package_index] : NULL;
}
