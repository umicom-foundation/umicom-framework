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
UmiStatus umi_language_intelligence_request_scheduler_init_checked(UmiLanguageIntelligenceRequestScheduler *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
