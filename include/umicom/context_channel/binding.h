/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/binding.h
 *
 * PURPOSE:
 *   Bind reusable panels to channels through explicit contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_BINDING_H
#define UMICOM_CONTEXT_CHANNEL_BINDING_H
#include "umicom/context_channel/payload.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_BINDING_MAX_ITEMS 256U
/**
 * Represent the context binding data shared with callers of this public contract.
 */
typedef struct UmiContextBinding {
    uint32_t structure_size;
    char binding_id[UMI_CONTEXT_TEXT_CAPACITY];
    char application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char accepted_schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    bool publish_enabled;
    bool observe_enabled;
    uint64_t revision;
} UmiContextBinding;
/**
 * Represent the context binding store data shared with callers of this public contract.
 */
typedef struct UmiContextBindingStore {
    UmiContextBinding items[UMI_CONTEXT_BINDING_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextBindingStore;
/**
 * Initialise context binding from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_binding_init(UmiContextBinding *record);
/**
 * Check that context binding satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_binding_validate(const UmiContextBinding *record);
/**
 * Initialise context binding store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_binding_store_init(UmiContextBindingStore *store);
/**
 * Provide the context binding store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_binding_store_put(UmiContextBindingStore *store,const UmiContextBinding *record);
/**
 * Remove context binding store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_binding_store_remove(UmiContextBindingStore *store,const char *identity);
/**
 * Find context binding store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextBinding *umi_context_binding_store_find(UmiContextBindingStore *store,const char *identity);
/**
 * Provide the context binding store find const operation used by this module and its
 * client applications.
 */
const UmiContextBinding *umi_context_binding_store_find_const(const UmiContextBindingStore *store,const char *identity);
/**
 * Return the number of records represented by context binding store without changing their
 * state.
 */
size_t umi_context_binding_store_count(const UmiContextBindingStore *store);
/**
 * Provide the context binding store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_binding_store_snapshot(const UmiContextBindingStore *store,UmiContextBinding *out_records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_binding_archive_encode(const UmiContextBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiContextBinding *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextBinding object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_binding_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextBinding *workspace, UmiContextBinding *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_binding_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextBinding *expected, UmiContextBinding *value);


#ifdef __cplusplus
}
#endif
#endif
