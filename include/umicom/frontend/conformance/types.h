/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/types.h
 *
 * PURPOSE:
 *   shared frontend-conformance identifiers, enums, bounded text and deterministic scoring helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_TYPES_H
#define UMICOM_FRONTEND_CONFORMANCE_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FC_ID_CAPACITY 128U
#define UMI_FC_TEXT_CAPACITY 256U
#define UMI_FC_MAX_ITEMS 256U
#define UMI_FC_MAX_LAYOUT_VALUES 32U
#define UMI_FC_MAX_TREE_NODES 256U

/**
 * List the named fc frontend kind values accepted by this public contract.
 */
typedef enum UmiFcFrontendKind {
    UMI_FC_FRONTEND_GTK4 = 1,
    UMI_FC_FRONTEND_QT6 = 2,
    UMI_FC_FRONTEND_NATIVE_WEB = 3,
    UMI_FC_FRONTEND_HEADLESS = 4
} UmiFcFrontendKind;

/**
 * List the named fc outcome values accepted by this public contract.
 */
typedef enum UmiFcOutcome { UMI_FC_PASS=1, UMI_FC_DEGRADED=2, UMI_FC_FAIL=3 } UmiFcOutcome;
/**
 * List the named fc severity values accepted by this public contract.
 */
typedef enum UmiFcSeverity { UMI_FC_INFO=1, UMI_FC_WARNING=2, UMI_FC_BLOCKER=3 } UmiFcSeverity;

/**
 * List the named fc capability kind values accepted by this public contract.
 */
typedef enum UmiFcCapabilityKind {
    UMI_FC_CAP_DOCK=0, UMI_FC_CAP_TABS=1, UMI_FC_CAP_SPLIT=2, UMI_FC_CAP_FLOAT=3,
    UMI_FC_CAP_AUTO_HIDE=4, UMI_FC_CAP_MULTI_MONITOR=5, UMI_FC_CAP_KEYBOARD=6,
    UMI_FC_CAP_FOCUS=7, UMI_FC_CAP_ACCESSIBILITY=8, UMI_FC_CAP_THEME=9,
    UMI_FC_CAP_DENSITY=10, UMI_FC_CAP_DRAG_DROP=11, UMI_FC_CAP_CONTEXT=12,
    UMI_FC_CAP_COMMAND=13, UMI_FC_CAP_EVENT=14, UMI_FC_CAP_PERSISTENCE=15,
    UMI_FC_CAP_RESPONSIVE=16, UMI_FC_CAP_REALTIME=17, UMI_FC_CAP_CHART=18,
    UMI_FC_CAP_TIMELINE=19, UMI_FC_CAP_MEDIA=20, UMI_FC_CAP_DATA_GRID=21,
    UMI_FC_CAP_NODE_GRAPH=22, UMI_FC_CAP_CANVAS=23, UMI_FC_CAP_COUNT=24
} UmiFcCapabilityKind;

/**
 * Provide the fc copy text operation used by this module and its client applications.
 */
UmiStatus umi_fc_copy_text(char *dst, size_t capacity, const char *src);
/**
 * Provide the fc hash text operation used by this module and its client applications.
 */
uint64_t umi_fc_hash_text(const char *text);
/**
 * Provide the fc hash combine operation used by this module and its client applications.
 */
uint64_t umi_fc_hash_combine(uint64_t seed, uint64_t value);
/**
 * Provide the fc ratio operation used by this module and its client applications.
 */
double umi_fc_ratio(size_t numerator, size_t denominator);
/**
 * Provide the fc clamp score operation used by this module and its client applications.
 */
double umi_fc_clamp_score(double score);
/**
 * Provide the fc frontend kind text operation used by this module and its client
 * applications.
 */
const char *umi_fc_frontend_kind_text(UmiFcFrontendKind kind);
/**
 * Provide the fc outcome text operation used by this module and its client applications.
 */
const char *umi_fc_outcome_text(UmiFcOutcome outcome);

#ifdef __cplusplus
}
#endif
#endif
