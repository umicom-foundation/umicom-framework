/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/server_trace.h
 *
 * PURPOSE:
 *   Maintain bounded language-server trace event identities.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_SERVER_TRACE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_SERVER_TRACE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_SERVER_TRACE_API_VERSION 1U
/**
 * Represent the language intelligence server trace item data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceServerTraceItem {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char label[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    uint64_t fingerprint;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceServerTraceItem;
/**
 * Represent the language intelligence server trace data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligenceServerTrace {
    uint32_t struct_size;
    uint32_t api_version;
    UmiLanguageIntelligenceServerTraceItem items[UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiLanguageIntelligenceServerTrace;
/**
 * Initialise language intelligence server trace from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_server_trace_init(UmiLanguageIntelligenceServerTrace *catalogue);
/**
 * Provide the language intelligence server trace upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_intelligence_server_trace_upsert(
    UmiLanguageIntelligenceServerTrace *catalogue,
    const UmiLanguageIntelligenceServerTraceItem *item);
/**
 * Find language intelligence server trace while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_find(
    const UmiLanguageIntelligenceServerTrace *catalogue,
    const char *id);
/**
 * Provide the language intelligence server trace best operation used by this module and
 * its client applications.
 */
const UmiLanguageIntelligenceServerTraceItem *umi_language_intelligence_server_trace_best(
    const UmiLanguageIntelligenceServerTrace *catalogue);
#ifdef __cplusplus
}
#endif
#endif
