/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/schema.h
 *
 * PURPOSE:
 *   Register typed context schemas and compatibility metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_SCHEMA_H
#define UMICOM_CONTEXT_CHANNEL_SCHEMA_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_SCHEMA_MAX_ITEMS 64U
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
typedef struct UmiContextSchemaStore {
    UmiContextSchema items[UMI_CONTEXT_SCHEMA_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextSchemaStore;
void umi_context_schema_init(UmiContextSchema *record);
UmiStatus umi_context_schema_validate(const UmiContextSchema *record);
void umi_context_schema_store_init(UmiContextSchemaStore *store);
UmiStatus umi_context_schema_store_put(UmiContextSchemaStore *store,const UmiContextSchema *record);
UmiStatus umi_context_schema_store_remove(UmiContextSchemaStore *store,const char *identity);
UmiContextSchema *umi_context_schema_store_find(UmiContextSchemaStore *store,const char *identity);
const UmiContextSchema *umi_context_schema_store_find_const(const UmiContextSchemaStore *store,const char *identity);
size_t umi_context_schema_store_count(const UmiContextSchemaStore *store);
UmiStatus umi_context_schema_store_snapshot(const UmiContextSchemaStore *store,UmiContextSchema *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
