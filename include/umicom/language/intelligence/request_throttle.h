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
typedef struct UmiLanguageIntelligenceRequestThrottle {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceRequestThrottle;
void umi_language_intelligence_request_throttle_init(UmiLanguageIntelligenceRequestThrottle *budget, uint64_t limit);
UmiStatus umi_language_intelligence_request_throttle_consume(UmiLanguageIntelligenceRequestThrottle *budget, uint64_t amount);
uint64_t umi_language_intelligence_request_throttle_remaining(const UmiLanguageIntelligenceRequestThrottle *budget);
void umi_language_intelligence_request_throttle_reset(UmiLanguageIntelligenceRequestThrottle *budget);
#ifdef __cplusplus
}
#endif
#endif
