/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_descriptor.h
 *
 * PURPOSE:
 *   Describe a model endpoint without coupling applications to provider-specific configuration.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_DESCRIPTOR_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_DESCRIPTOR_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model descriptor data shared with callers of this public contract.
 */
typedef struct UmiAiDevModelDescriptor {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevModelDescriptor;

/**
 * Initialise ai dev model descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_descriptor_init(UmiAiDevModelDescriptor *value);
/**
 * Provide the ai dev model descriptor configure operation used by this module and its
 * client applications.
 *
 * Text inputs may refer to the destination's current inline fields. A temporary
 * value is allocated and published only on success; invalid input or allocation
 * failure leaves the destination unchanged. NULL label becomes empty and long
 * text keeps the existing bounded-truncation behaviour. Successful configuration
 * starts a new metadata value at revision 1; it is not a live-service update and
 * grants no execution authority. Callers serialize access to the destination.
 */
UmiStatus umi_ai_dev_model_descriptor_configure(UmiAiDevModelDescriptor *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev model descriptor satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ai_dev_model_descriptor_validate(const UmiAiDevModelDescriptor *value);
/**
 * Provide the ai dev model descriptor evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_model_descriptor_evidence_score(const UmiAiDevModelDescriptor *value, uint32_t relevance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ai_dev_model_descriptor_archive_encode(const UmiAiDevModelDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ai_dev_model_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiAiDevModelDescriptor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAiDevModelDescriptor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ai_dev_model_descriptor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAiDevModelDescriptor *workspace, UmiAiDevModelDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ai_dev_model_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAiDevModelDescriptor *expected, UmiAiDevModelDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
