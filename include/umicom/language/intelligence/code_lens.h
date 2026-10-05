/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/code_lens.h
 *
 * PURPOSE:
 *   Represent code-lens commands without toolkit or LSP transport ownership.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H
#define UMICOM_LANGUAGE_INTELLIGENCE_CODE_LENS_H

#include "umicom/language/intelligence/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_LANGUAGE_INTELLIGENCE_CODE_LENS_API_VERSION 1U

/**
 * Represent the language intelligence code lens data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageIntelligenceCodeLens {
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
} UmiLanguageIntelligenceCodeLens;

/**
 * Initialise language intelligence code lens from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_code_lens_init(
    UmiLanguageIntelligenceCodeLens *value,
    const char *id);
/**
 * Check that language intelligence code lens satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_language_intelligence_code_lens_validate(
    const UmiLanguageIntelligenceCodeLens *value);
/**
 * Provide the language intelligence code lens set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_intelligence_code_lens_set_subject(
    UmiLanguageIntelligenceCodeLens *value,
    const char *subject_id);
/**
 * Provide the language intelligence code lens set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_intelligence_code_lens_set_detail(
    UmiLanguageIntelligenceCodeLens *value,
    const char *detail);
/**
 * Provide the language intelligence code lens touch operation used by this module and its
 * client applications.
 */
void umi_language_intelligence_code_lens_touch(UmiLanguageIntelligenceCodeLens *value);
/**
 * Provide the language intelligence code lens same identity operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_code_lens_same_identity(
    const UmiLanguageIntelligenceCodeLens *left,
    const UmiLanguageIntelligenceCodeLens *right);

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
UmiStatus umi_language_intelligence_code_lens_init_checked(UmiLanguageIntelligenceCodeLens *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_language_intelligence_code_lens_archive_encode(const UmiLanguageIntelligenceCodeLens *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_language_intelligence_code_lens_archive_decode(const void *bytes, size_t byte_count,
    UmiLanguageIntelligenceCodeLens *value);

#ifdef __cplusplus
}
#endif
#endif
