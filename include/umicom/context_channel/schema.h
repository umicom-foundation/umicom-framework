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
#ifdef __cplusplus
}
#endif
#endif
