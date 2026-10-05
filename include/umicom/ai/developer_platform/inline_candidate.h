/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/inline_candidate.h
 *
 * PURPOSE:
 *   Describe a candidate inline edit without applying it.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_INLINE_CANDIDATE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_INLINE_CANDIDATE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev inline candidate data shared with callers of this public contract.
 */
typedef struct UmiAiDevInlineCandidate {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevInlineCandidate;

/**
 * Initialise ai dev inline candidate from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_inline_candidate_init(UmiAiDevInlineCandidate *value);
/**
 * Provide the ai dev inline candidate configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_inline_candidate_configure(UmiAiDevInlineCandidate *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev inline candidate satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_dev_inline_candidate_validate(const UmiAiDevInlineCandidate *value);
/**
 * Provide the ai dev inline candidate evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_inline_candidate_evidence_score(const UmiAiDevInlineCandidate *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ai_dev_inline_candidate_archive_encode(const UmiAiDevInlineCandidate *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ai_dev_inline_candidate_archive_decode(const void *bytes, size_t byte_count,
    UmiAiDevInlineCandidate *value);

#ifdef __cplusplus
}
#endif

#endif
