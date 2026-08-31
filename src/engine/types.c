/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/engine/types.c
 *
 * PURPOSE:
 *   Convert reusable engine enumerations into stable human-readable text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/engine/types.h"

/* Convert a stable engine kind into text without allocating memory. */
const char *umi_engine_kind_text(UmiEngineKind kind)
{
    /* The switch keeps persisted numeric values independent from display text. */
    switch (kind) {
        case UMI_ENGINE_GAME: return "game";
        case UMI_ENGINE_RENDER_3D: return "render-3d";
        case UMI_ENGINE_CHART_ANALYTICS: return "chart-analytics";
        case UMI_ENGINE_DOCUMENT_GENERATION: return "document-generation";
        case UMI_ENGINE_MEDIA_RENDERING: return "media-rendering";
        case UMI_ENGINE_TRADING_MATCHING: return "trading-matching";
        case UMI_ENGINE_SOCIAL_COMMUNITY: return "social-community";
        case UMI_ENGINE_DATA_CONNECTIVITY: return "data-connectivity";
        default: return "unknown";
    }
}

/* Convert an engine maturity level into text without overstating readiness. */
const char *umi_engine_maturity_text(UmiEngineMaturity maturity)
{
    /* Unknown values are handled explicitly for forward-compatible diagnostics. */
    switch (maturity) {
        case UMI_ENGINE_MATURITY_CONTRACT: return "contract";
        case UMI_ENGINE_MATURITY_FOUNDATION: return "foundation";
        case UMI_ENGINE_MATURITY_OPERATIONAL: return "operational";
        default: return "unknown";
    }
}
