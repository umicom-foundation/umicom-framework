/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/request_scheduler.h
 *
 * PURPOSE:
 *   Represent scheduled language-intelligence work and its ordering metadata.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_API_VERSION 1U

typedef struct UmiLanguageIntelligenceRequestScheduler {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char subject_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char detail[UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY];
    UmiLanguageIntelligenceRange range;
    uint64_t revision;
    uint32_t priority;
    uint32_t flags;
    int enabled;
} UmiLanguageIntelligenceRequestScheduler;

void umi_language_intelligence_request_scheduler_init(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *id);
UmiStatus umi_language_intelligence_request_scheduler_validate(
    const UmiLanguageIntelligenceRequestScheduler *value);
UmiStatus umi_language_intelligence_request_scheduler_set_subject(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_request_scheduler_set_detail(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *detail);
void umi_language_intelligence_request_scheduler_touch(UmiLanguageIntelligenceRequestScheduler *value);
int umi_language_intelligence_request_scheduler_same_identity(
    const UmiLanguageIntelligenceRequestScheduler *left,
    const UmiLanguageIntelligenceRequestScheduler *right);

#ifdef __cplusplus
}
#endif
#endif
