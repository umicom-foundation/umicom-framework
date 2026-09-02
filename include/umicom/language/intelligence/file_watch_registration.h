/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/file_watch_registration.h
 *
 * PURPOSE:
 *   Describe Framework-owned watched-file registrations for language providers.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_API_VERSION 1U

/**
 * Represent the language intelligence file watch registration data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceFileWatchRegistration {
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
} UmiLanguageIntelligenceFileWatchRegistration;

/**
 * Initialise language intelligence file watch registration from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_file_watch_registration_init(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *id);
/**
 * Check that language intelligence file watch registration satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_language_intelligence_file_watch_registration_validate(
    const UmiLanguageIntelligenceFileWatchRegistration *value);
/**
 * Provide the language intelligence file watch registration set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_file_watch_registration_set_subject(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *subject_id);
/**
 * Provide the language intelligence file watch registration set detail operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_file_watch_registration_set_detail(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *detail);
/**
 * Provide the language intelligence file watch registration touch operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_file_watch_registration_touch(UmiLanguageIntelligenceFileWatchRegistration *value);
/**
 * Provide the language intelligence file watch registration same identity operation used
 * by this module and its client applications.
 */
int umi_language_intelligence_file_watch_registration_same_identity(
    const UmiLanguageIntelligenceFileWatchRegistration *left,
    const UmiLanguageIntelligenceFileWatchRegistration *right);

#ifdef __cplusplus
}
#endif
#endif
