/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_source.h
 *
 * PURPOSE:
 *   Describe a source of developer context such as repository files, diagnostics, history or symbols.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_SOURCE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_SOURCE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context source data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextSource {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevContextSource;

/**
 * Initialise ai dev context source from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_source_init(UmiAiDevContextSource *value);
/**
 * Provide the ai dev context source configure operation used by this module and its client
 * applications.
 *
 * Text inputs may refer to the destination's current inline fields. A temporary
 * value is allocated and published only on success; invalid input or allocation
 * failure leaves the destination unchanged. NULL label becomes empty and long
 * text keeps the existing bounded-truncation behaviour. Successful configuration
 * starts a new metadata value at revision 1; it is not a live-service update and
 * grants no execution authority. Callers serialize access to the destination.
 */
UmiStatus umi_ai_dev_context_source_configure(UmiAiDevContextSource *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev context source satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_context_source_validate(const UmiAiDevContextSource *value);
/**
 * Provide the ai dev context source evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_context_source_evidence_score(const UmiAiDevContextSource *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ai_dev_context_source_archive_encode(const UmiAiDevContextSource *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ai_dev_context_source_archive_decode(const void *bytes, size_t byte_count,
    UmiAiDevContextSource *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ai_dev_context_source_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAiDevContextSource *expected, UmiAiDevContextSource *value);


#ifdef __cplusplus
}
#endif

#endif
