/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/engine/types.h
 *
 * PURPOSE:
 *   Define the shared vocabulary used to describe reusable Umicom engines and
 *   the capabilities that each engine can safely provide to applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENGINE_TYPES_H
#define UMICOM_ENGINE_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ENGINE_API_VERSION 1U

/* Engine kinds are stable identifiers; applications should not invent their
 * own numbers because persisted capability profiles may contain these values. */
typedef enum UmiEngineKind {
    UMI_ENGINE_GAME = 0,
    UMI_ENGINE_RENDER_3D = 1,
    UMI_ENGINE_CHART_ANALYTICS = 2,
    UMI_ENGINE_DOCUMENT_GENERATION = 3,
    UMI_ENGINE_MEDIA_RENDERING = 4,
    UMI_ENGINE_TRADING_MATCHING = 5,
    UMI_ENGINE_SOCIAL_COMMUNITY = 6,
    UMI_ENGINE_DATA_CONNECTIVITY = 7
} UmiEngineKind;

/* Maturity describes usable implementation depth and prevents a declaration
 * or adapter boundary from being presented as a finished production engine. */
typedef enum UmiEngineMaturity {
    UMI_ENGINE_MATURITY_CONTRACT = 0,
    UMI_ENGINE_MATURITY_FOUNDATION = 1,
    UMI_ENGINE_MATURITY_OPERATIONAL = 2
} UmiEngineMaturity;

typedef uint64_t UmiEngineCapabilityMask;

#define UMI_ENGINE_CAPABILITY_CATALOGUE          (UINT64_C(1) << 0U)
#define UMI_ENGINE_CAPABILITY_ENTITY_WORLD       (UINT64_C(1) << 1U)
#define UMI_ENGINE_CAPABILITY_SESSION_SYNC       (UINT64_C(1) << 2U)
#define UMI_ENGINE_CAPABILITY_SCENE_GRAPH        (UINT64_C(1) << 3U)
#define UMI_ENGINE_CAPABILITY_IMAGE_SURFACE      (UINT64_C(1) << 4U)
#define UMI_ENGINE_CAPABILITY_ANIMATION_TIMELINE (UINT64_C(1) << 5U)
#define UMI_ENGINE_CAPABILITY_CHART_WORKSPACE    (UINT64_C(1) << 6U)
#define UMI_ENGINE_CAPABILITY_ANALYTICS          (UINT64_C(1) << 7U)
#define UMI_ENGINE_CAPABILITY_TEXT_OUTPUT        (UINT64_C(1) << 8U)
#define UMI_ENGINE_CAPABILITY_OUTPUT_ADAPTER     (UINT64_C(1) << 9U)
#define UMI_ENGINE_CAPABILITY_ORDER_MATCHING     (UINT64_C(1) << 10U)
#define UMI_ENGINE_CAPABILITY_RISK_CONTROLS      (UINT64_C(1) << 11U)
#define UMI_ENGINE_CAPABILITY_COMMUNITY_STREAM   (UINT64_C(1) << 12U)
#define UMI_ENGINE_CAPABILITY_MODERATION         (UINT64_C(1) << 13U)
#define UMI_ENGINE_CAPABILITY_SQL_CONNECTOR      (UINT64_C(1) << 14U)
#define UMI_ENGINE_CAPABILITY_NOSQL_CONNECTOR    (UINT64_C(1) << 15U)

/* Return stable text for logs, diagnostics and beginner-facing engine lists. */
const char *umi_engine_kind_text(UmiEngineKind kind);

/* Return truthful maturity text so a foundation is not called operational. */
const char *umi_engine_maturity_text(UmiEngineMaturity maturity);

#ifdef __cplusplus
}
#endif

#endif
