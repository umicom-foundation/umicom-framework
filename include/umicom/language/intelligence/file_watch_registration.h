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

/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_language_intelligence_file_watch_registration_init_checked(UmiLanguageIntelligenceFileWatchRegistration *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
