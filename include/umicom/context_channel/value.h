/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/value.h
 *
 * PURPOSE:
 *   Represent named typed values carried by generic context payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_VALUE_H
#define UMICOM_CONTEXT_CHANNEL_VALUE_H
#include "umicom/context_channel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context value data shared with callers of this public contract.
 */
typedef struct UmiContextValue {
    uint32_t structure_size;
    UmiContextValueKind kind;
    char name[UMI_CONTEXT_TEXT_CAPACITY];
    char text[UMI_CONTEXT_VALUE_CAPACITY];
    int64_t integer_value;
    uint64_t unsigned_value;
    double decimal_value;
    bool boolean_value;
} UmiContextValue;
/**
 * Initialise context value from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_value_init(UmiContextValue *value, const char *name);
/**
 * Provide the context value set text operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_text(UmiContextValue *value, const char *text);
/**
 * Provide the context value set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_integer(UmiContextValue *value, int64_t number);
/**
 * Provide the context value set unsigned operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_unsigned(UmiContextValue *value, uint64_t number);
/**
 * Provide the context value set decimal operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_decimal(UmiContextValue *value, double number);
/**
 * Provide the context value set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_value_set_boolean(UmiContextValue *value, bool state);
/**
 * Check that context value satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_value_validate(const UmiContextValue *value);
/**
 * Provide the context value equal operation used by this module and its client
 * applications.
 */
bool umi_context_value_equal(const UmiContextValue *left, const UmiContextValue *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_value_archive_encode(const UmiContextValue *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_value_archive_decode(const void *bytes, size_t byte_count,
    UmiContextValue *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextValue object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_value_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextValue *workspace, UmiContextValue *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_value_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextValue *expected, UmiContextValue *value);


#ifdef __cplusplus
}
#endif
#endif
