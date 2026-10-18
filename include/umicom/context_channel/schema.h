/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/schema.h
 *
 * PURPOSE:
 *   Register typed context schemas and compatibility metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SCHEMA_H
#define UMICOM_CONTEXT_CHANNEL_SCHEMA_H
#include "umicom/context_channel/payload.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SCHEMA_MAX_ITEMS 64U
/**
 * Represent the context schema data shared with callers of this public contract.
 */
typedef struct UmiContextSchema {
    uint32_t structure_size;
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char display_name[UMI_CONTEXT_TEXT_CAPACITY];
    char description[UMI_CONTEXT_VALUE_CAPACITY];
    UmiContextKind kind;
    uint32_t schema_version;
    uint32_t minimum_compatible_version;
    bool sensitive;
    uint64_t revision;
} UmiContextSchema;
/**
 * Represent the context schema store data shared with callers of this public contract.
 */
typedef struct UmiContextSchemaStore {
    UmiContextSchema items[UMI_CONTEXT_SCHEMA_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextSchemaStore;
/**
 * Initialise context schema from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_schema_init(UmiContextSchema *record);
/**
 * Check that context schema satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_schema_validate(const UmiContextSchema *record);
/**
 * Initialise context schema store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_schema_store_init(UmiContextSchemaStore *store);
/**
 * Provide the context schema store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_schema_store_put(UmiContextSchemaStore *store,const UmiContextSchema *record);
/**
 * Remove context schema store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_schema_store_remove(UmiContextSchemaStore *store,const char *identity);
/**
 * Find context schema store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextSchema *umi_context_schema_store_find(UmiContextSchemaStore *store,const char *identity);
/**
 * Provide the context schema store find const operation used by this module and its client
 * applications.
 */
const UmiContextSchema *umi_context_schema_store_find_const(const UmiContextSchemaStore *store,const char *identity);
/**
 * Return the number of records represented by context schema store without changing their
 * state.
 */
size_t umi_context_schema_store_count(const UmiContextSchemaStore *store);
/**
 * Provide the context schema store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_schema_store_snapshot(const UmiContextSchemaStore *store,UmiContextSchema *out_records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_schema_archive_encode(const UmiContextSchema *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_schema_archive_decode(const void *bytes, size_t byte_count,
    UmiContextSchema *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiContextSchema object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_context_schema_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiContextSchema *workspace, UmiContextSchema *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_schema_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextSchema *expected, UmiContextSchema *value);


#ifdef __cplusplus
}
#endif
#endif
