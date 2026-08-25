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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_H

#include "umicom/language/intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_FILE_WATCH_REGISTRATION_API_VERSION 1U

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

void umi_language_intelligence_file_watch_registration_init(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *id);
UmiStatus umi_language_intelligence_file_watch_registration_validate(
    const UmiLanguageIntelligenceFileWatchRegistration *value);
UmiStatus umi_language_intelligence_file_watch_registration_set_subject(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *subject_id);
UmiStatus umi_language_intelligence_file_watch_registration_set_detail(
    UmiLanguageIntelligenceFileWatchRegistration *value,
    const char *detail);
void umi_language_intelligence_file_watch_registration_touch(UmiLanguageIntelligenceFileWatchRegistration *value);
int umi_language_intelligence_file_watch_registration_same_identity(
    const UmiLanguageIntelligenceFileWatchRegistration *left,
    const UmiLanguageIntelligenceFileWatchRegistration *right);

#ifdef __cplusplus
}
#endif
#endif
