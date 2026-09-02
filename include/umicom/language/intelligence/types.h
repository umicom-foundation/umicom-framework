/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/types.h
 *
 * PURPOSE:
 *   Shared native language-intelligence types, bounded text helpers, ranges and stable state vocabulary.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_TYPES_H
#define UMICOM_LANGUAGE_INTELLIGENCE_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_API_VERSION 1U
#define UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY 128U
#define UMI_LANGUAGE_INTELLIGENCE_TEXT_CAPACITY 256U
#define UMI_LANGUAGE_INTELLIGENCE_PATH_CAPACITY 2048U
#define UMI_LANGUAGE_INTELLIGENCE_ITEM_CAPACITY 64U

typedef uint64_t UmiLanguageIntelligenceCapabilityFlags;
enum {
    UMI_LANGUAGE_INTELLIGENCE_CAP_COMPLETION = UINT64_C(1) << 0,
    UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION = UINT64_C(1) << 1,
    UMI_LANGUAGE_INTELLIGENCE_CAP_DIAGNOSTICS = UINT64_C(1) << 2,
    UMI_LANGUAGE_INTELLIGENCE_CAP_FORMATTING = UINT64_C(1) << 3,
    UMI_LANGUAGE_INTELLIGENCE_CAP_REFACTORING = UINT64_C(1) << 4,
    UMI_LANGUAGE_INTELLIGENCE_CAP_SEMANTIC = UINT64_C(1) << 5,
    UMI_LANGUAGE_INTELLIGENCE_CAP_HIERARCHY = UINT64_C(1) << 6,
    UMI_LANGUAGE_INTELLIGENCE_CAP_WORKSPACE = UINT64_C(1) << 7,
    UMI_LANGUAGE_INTELLIGENCE_CAP_DYNAMIC_REGISTRATION = UINT64_C(1) << 8,
    UMI_LANGUAGE_INTELLIGENCE_CAP_PULL_DIAGNOSTICS = UINT64_C(1) << 9,
    UMI_LANGUAGE_INTELLIGENCE_CAP_NOTEBOOK = UINT64_C(1) << 10,
    UMI_LANGUAGE_INTELLIGENCE_CAP_ALL =
        UMI_LANGUAGE_INTELLIGENCE_CAP_COMPLETION |
        UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION |
        UMI_LANGUAGE_INTELLIGENCE_CAP_DIAGNOSTICS |
        UMI_LANGUAGE_INTELLIGENCE_CAP_FORMATTING |
        UMI_LANGUAGE_INTELLIGENCE_CAP_REFACTORING |
        UMI_LANGUAGE_INTELLIGENCE_CAP_SEMANTIC |
        UMI_LANGUAGE_INTELLIGENCE_CAP_HIERARCHY |
        UMI_LANGUAGE_INTELLIGENCE_CAP_WORKSPACE |
        UMI_LANGUAGE_INTELLIGENCE_CAP_DYNAMIC_REGISTRATION |
        UMI_LANGUAGE_INTELLIGENCE_CAP_PULL_DIAGNOSTICS |
        UMI_LANGUAGE_INTELLIGENCE_CAP_NOTEBOOK
};

/**
 * List the named language intelligence state values accepted by this public contract.
 */
typedef enum UmiLanguageIntelligenceState {
    UMI_LANGUAGE_INTELLIGENCE_UNKNOWN = 0,
    UMI_LANGUAGE_INTELLIGENCE_STARTING = 1,
    UMI_LANGUAGE_INTELLIGENCE_READY = 2,
    UMI_LANGUAGE_INTELLIGENCE_DEGRADED = 3,
    UMI_LANGUAGE_INTELLIGENCE_QUARANTINED = 4,
    UMI_LANGUAGE_INTELLIGENCE_STOPPED = 5,
    UMI_LANGUAGE_INTELLIGENCE_FAILED = 6
} UmiLanguageIntelligenceState;

/**
 * List the named language intelligence severity values accepted by this public contract.
 */
typedef enum UmiLanguageIntelligenceSeverity {
    UMI_LANGUAGE_INTELLIGENCE_SEVERITY_INFORMATION = 0,
    UMI_LANGUAGE_INTELLIGENCE_SEVERITY_WARNING = 1,
    UMI_LANGUAGE_INTELLIGENCE_SEVERITY_ERROR = 2,
    UMI_LANGUAGE_INTELLIGENCE_SEVERITY_FATAL = 3
} UmiLanguageIntelligenceSeverity;

/**
 * Represent the language intelligence position data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligencePosition {
    uint32_t line;
    uint32_t character;
} UmiLanguageIntelligencePosition;

/**
 * Represent the language intelligence range data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligenceRange {
    UmiLanguageIntelligencePosition start;
    UmiLanguageIntelligencePosition end;
} UmiLanguageIntelligenceRange;

/**
 * Represent the language intelligence scored id data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligenceScoredId {
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint64_t score;
    uint32_t priority;
    int enabled;
} UmiLanguageIntelligenceScoredId;

/**
 * Provide the language intelligence types init range operation used by this module and its
 * client applications.
 */
void umi_language_intelligence_types_init_range(
    UmiLanguageIntelligenceRange *range,
    uint32_t start_line,
    uint32_t start_character,
    uint32_t end_line,
    uint32_t end_character);
/**
 * Check that language intelligence range satisfies its contract before another service
 * relies on it.
 */
int umi_language_intelligence_range_is_valid(
    const UmiLanguageIntelligenceRange *range);
/**
 * Provide the language intelligence range contains operation used by this module and its
 * client applications.
 */
int umi_language_intelligence_range_contains(
    const UmiLanguageIntelligenceRange *outer,
    const UmiLanguageIntelligenceRange *inner);
/**
 * Provide the language intelligence copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_intelligence_copy_text(
    char *destination,
    size_t capacity,
    const char *source);
/**
 * Check that language intelligence text satisfies its contract before another service
 * relies on it.
 */
int umi_language_intelligence_text_is_valid(const char *text);
/**
 * Provide the language intelligence hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_language_intelligence_hash_text(const char *text);
/**
 * Add language intelligence score saturating only after its inputs and available capacity
 * have been checked.
 */
uint64_t umi_language_intelligence_score_saturating_add(
    uint64_t left,
    uint64_t right);
/**
 * Provide the language intelligence state text operation used by this module and its
 * client applications.
 */
const char *umi_language_intelligence_state_text(
    UmiLanguageIntelligenceState state);
/**
 * Provide the language intelligence severity text operation used by this module and its
 * client applications.
 */
const char *umi_language_intelligence_severity_text(
    UmiLanguageIntelligenceSeverity severity);

#ifdef __cplusplus
}
#endif
#endif
