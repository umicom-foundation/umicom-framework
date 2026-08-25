/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_cancellation.h
 *
 * PURPOSE:
 *   Track cancellation state across asynchronous provider requests.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_CANCELLATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_CANCELLATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_CANCELLATION_API_VERSION 1U
typedef struct UmiLanguageIntelligenceRequestCancellation {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceRequestCancellation;
void umi_language_intelligence_request_cancellation_init(UmiLanguageIntelligenceRequestCancellation *value, const char *id);
UmiStatus umi_language_intelligence_request_cancellation_transition(
    UmiLanguageIntelligenceRequestCancellation *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_request_cancellation_record_failure(UmiLanguageIntelligenceRequestCancellation *value);
int umi_language_intelligence_request_cancellation_is_usable(const UmiLanguageIntelligenceRequestCancellation *value);
#ifdef __cplusplus
}
#endif
#endif
