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
typedef struct UmiLanguageIntelligenceRequestDeadline {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t limit;
    uint64_t used;
    uint64_t peak;
    uint64_t revision;
    int exhausted;
} UmiLanguageIntelligenceRequestDeadline;
void umi_language_intelligence_request_deadline_init(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t limit);
UmiStatus umi_language_intelligence_request_deadline_consume(UmiLanguageIntelligenceRequestDeadline *budget, uint64_t amount);
uint64_t umi_language_intelligence_request_deadline_remaining(const UmiLanguageIntelligenceRequestDeadline *budget);
void umi_language_intelligence_request_deadline_reset(UmiLanguageIntelligenceRequestDeadline *budget);
#ifdef __cplusplus
}
#endif
#endif
