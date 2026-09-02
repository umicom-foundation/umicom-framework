/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_deadline.h
 *
 * PURPOSE:
 *   Enforce request deadlines without provider-specific timeout logic in applications.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_DEADLINE_API_VERSION 1U
/**
 * Represent the language intelligence request deadline data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceRequestDeadline {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceRequestDeadline;
/**
 * Initialise language intelligence request deadline from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_request_deadline_init(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t limit);
/**
 * Provide the language intelligence request deadline consume operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_request_deadline_consume(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t amount);
/**
 * Provide the language intelligence request deadline remaining operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_request_deadline_remaining(const UmiLanguageIntelligenceRequestDeadline *budget);
/**
 * Release or reset state held by language intelligence request deadline so the same
 * storage can be reused safely.
 */
void umi_language_intelligence_request_deadline_reset(UmiLanguageIntelligenceRequestDeadline *budget);
#ifdef __cplusplus
}
#endif
#endif
