/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_throttle.h
 *
 * PURPOSE:
 *   Apply bounded rate limits to noisy language-provider operations.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_THROTTLE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_THROTTLE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_THROTTLE_API_VERSION 1U
/**
 * Represent the language intelligence request throttle data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceRequestThrottle {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceRequestThrottle;
/**
 * Initialise language intelligence request throttle from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_request_throttle_init(UmiLanguageIntelligenceRequestThrottle *budget, uint64_t limit);
/**
 * Provide the language intelligence request throttle consume operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_request_throttle_consume(UmiLanguageIntelligenceRequestThrottle *budget, uint64_t amount);
/**
 * Provide the language intelligence request throttle remaining operation used by this
 * module and its client applications.
 */
uint64_t umi_language_intelligence_request_throttle_remaining(const UmiLanguageIntelligenceRequestThrottle *budget);
/**
 * Release or reset state held by language intelligence request throttle so the same
 * storage can be reused safely.
 */
void umi_language_intelligence_request_throttle_reset(UmiLanguageIntelligenceRequestThrottle *budget);
#ifdef __cplusplus
}
#endif
#endif
