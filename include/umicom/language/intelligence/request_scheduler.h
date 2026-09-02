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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_H
#define UMICOM_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_REQUEST_SCHEDULER_API_VERSION 1U

/**
 * Represent the language intelligence request scheduler data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise language intelligence request scheduler from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_request_scheduler_init(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *id);
/**
 * Check that language intelligence request scheduler satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_language_intelligence_request_scheduler_validate(
    const UmiLanguageIntelligenceRequestScheduler *value);
/**
 * Provide the language intelligence request scheduler set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_request_scheduler_set_subject(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *subject_id);
/**
 * Provide the language intelligence request scheduler set detail operation used by this
 * module and its client applications.
 */
UmiStatus umi_language_intelligence_request_scheduler_set_detail(
    UmiLanguageIntelligenceRequestScheduler *value,
    const char *detail);
/**
 * Provide the language intelligence request scheduler touch operation used by this module
 * and its client applications.
 */
void umi_language_intelligence_request_scheduler_touch(UmiLanguageIntelligenceRequestScheduler *value);
/**
 * Provide the language intelligence request scheduler same identity operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_request_scheduler_same_identity(
    const UmiLanguageIntelligenceRequestScheduler *left,
    const UmiLanguageIntelligenceRequestScheduler *right);

#ifdef __cplusplus
}
#endif
#endif
