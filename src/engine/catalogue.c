/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/engine/catalogue.c
 *
 * PURPOSE:
 *   Implement the canonical catalogue and compatibility checks for reusable
 *   Umicom engines without constructing application-specific dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/engine/catalogue.h"

#include <string.h>

#define UMI_COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/* The catalogue reports only capabilities backed by code in the Framework.
 * Planned GPU, file-format and provider adapters are deliberately excluded. */
static const UmiEngineDescriptor UMI_ENGINES[] = {
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION, UMI_ENGINE_GAME,
     "umicom.game", "Game Engine",
     "Entity worlds and deterministic multiplayer session state.",
     "umicom/game/game.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_ENTITY_WORLD |
         UMI_ENGINE_CAPABILITY_SESSION_SYNC},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION, UMI_ENGINE_RENDER_3D,
     "umicom.render3d", "3D Rendering Engine",
     "Toolkit-neutral transforms, scene nodes and safe scene ownership.",
     "umicom/render3d/render3d.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_SCENE_GRAPH},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_CHART_ANALYTICS, "umicom.chart", "Chart and Analytics Engine",
     "Real-time chart workspaces, indicators, annotations and streams.",
     "umicom/chart/workspace.h", UMI_ENGINE_MATURITY_OPERATIONAL,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_CHART_WORKSPACE |
         UMI_ENGINE_CAPABILITY_ANALYTICS},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_DOCUMENT_GENERATION, "umicom.document-generation",
     "Document Generation Engine",
     "Output planning, safe text generation and format-adapter boundaries.",
     "umicom/document/generation.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_TEXT_OUTPUT |
         UMI_ENGINE_CAPABILITY_OUTPUT_ADAPTER},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_MEDIA_RENDERING, "umicom.media", "Media Rendering Engine",
     "Owned image surfaces and deterministic animation tracks.",
     "umicom/media/media.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_IMAGE_SURFACE |
         UMI_ENGINE_CAPABILITY_ANIMATION_TIMELINE},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_TRADING_MATCHING, "umicom.trading", "Trading Engine",
     "Order books, matching, brokerage, risk, surveillance and replay.",
     "umicom/trading/trading.h", UMI_ENGINE_MATURITY_OPERATIONAL,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_ORDER_MATCHING |
         UMI_ENGINE_CAPABILITY_RISK_CONTROLS},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_SOCIAL_COMMUNITY, "umicom.social", "Social Community Engine",
     "Bounded community membership, channels and message evidence.",
     "umicom/social/social.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_COMMUNITY_STREAM |
         UMI_ENGINE_CAPABILITY_MODERATION},
    {sizeof(UmiEngineDescriptor), UMI_ENGINE_API_VERSION,
     UMI_ENGINE_DATA_CONNECTIVITY, "umicom.data", "Data Connectivity Engine",
     "Provider-neutral SQL and NoSQL adapter contracts with safe credentials.",
     "umicom/data/connector.h", UMI_ENGINE_MATURITY_FOUNDATION,
     UMI_ENGINE_CAPABILITY_CATALOGUE |
         UMI_ENGINE_CAPABILITY_SQL_CONNECTOR |
         UMI_ENGINE_CAPABILITY_NOSQL_CONNECTOR}
};

/* Report catalogue size from the array itself so the count cannot drift. */
size_t umi_engine_catalogue_count(void)
{
    return UMI_COUNT_OF(UMI_ENGINES);
}

/* Return a borrowed catalogue entry while protecting the array boundary. */
const UmiEngineDescriptor *umi_engine_catalogue_at(size_t index)
{
    return index < UMI_COUNT_OF(UMI_ENGINES) ? &UMI_ENGINES[index] : NULL;
}

/* Locate an engine by its stable kind for application requirement checks. */
const UmiEngineDescriptor *umi_engine_catalogue_find(UmiEngineKind kind)
{
    size_t index;
    for (index = 0U; index < UMI_COUNT_OF(UMI_ENGINES); ++index) {
        if (UMI_ENGINES[index].kind == kind) return &UMI_ENGINES[index];
    }
    return NULL;
}

/* Locate an engine by its stable text identifier used in manifests and UI. */
const UmiEngineDescriptor *umi_engine_catalogue_find_id(const char *engine_id)
{
    size_t index;
    /* Empty identifiers cannot name an engine and are rejected before strcmp. */
    if (engine_id == NULL || engine_id[0] == '\0') return NULL;
    for (index = 0U; index < UMI_COUNT_OF(UMI_ENGINES); ++index) {
        if (strcmp(UMI_ENGINES[index].engine_id, engine_id) == 0) {
            return &UMI_ENGINES[index];
        }
    }
    return NULL;
}

/* Confirm that all requested bits are present, not merely one of them. */
bool umi_engine_descriptor_supports(
    const UmiEngineDescriptor *descriptor,
    UmiEngineCapabilityMask capabilities)
{
    return descriptor != NULL &&
        (descriptor->capabilities & capabilities) == capabilities;
}

/* Compare application requirements with truthful catalogue maturity and bits. */
UmiStatus umi_engine_catalogue_validate(
    const UmiEngineRequirement *requirements,
    size_t requirement_count,
    UmiEngineRequirementReport *out_report)
{
    size_t index;
    /* A NULL list is valid only when its declared count is zero. */
    if (out_report == NULL ||
        (requirement_count > 0U && requirements == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->struct_size = (uint32_t)sizeof(*out_report);
    out_report->api_version = UMI_ENGINE_API_VERSION;
    out_report->requirement_count = requirement_count;
    out_report->ready = true;

    /* Every requirement is evaluated so the report can show a useful total. */
    for (index = 0U; index < requirement_count; ++index) {
        const UmiEngineRequirement *requirement = &requirements[index];
        const UmiEngineDescriptor *descriptor =
            umi_engine_catalogue_find(requirement->kind);
        bool satisfied = descriptor != NULL &&
            descriptor->maturity >= requirement->minimum_maturity &&
            umi_engine_descriptor_supports(
                descriptor, requirement->required_capabilities);
        /* A requirement passes only when both maturity and capabilities pass. */
        if (satisfied) {
            out_report->satisfied_count += 1U;
        } else {
            UmiEngineCapabilityMask available = descriptor != NULL
                ? descriptor->capabilities : UINT64_C(0);
            /* Preserve the first failure as the most direct diagnostic hint. */
            if (out_report->missing_count == 0U) {
                out_report->first_missing_kind = requirement->kind;
                out_report->first_missing_capabilities =
                    requirement->required_capabilities & ~available;
            }
            out_report->missing_count += 1U;
            out_report->ready = false;
        }
    }
    return UMI_STATUS_OK;
}
