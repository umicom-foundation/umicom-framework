/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_index.c
 *
 * PURPOSE:
 *   Describe an index and its uniqueness/coverage characteristics for compatibility and planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/schema_index.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_schema_index_init(UmiDataSchemaIndex *item, const char *index_id, const char *table_id, const char *key_expression, bool unique) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->index_id,sizeof(item->index_id),index_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->table_id,sizeof(item->table_id),table_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->key_expression,sizeof(item->key_expression),key_expression); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; item->unique=unique; item->covering=false;
    return umi_data_schema_index_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_schema_index_init_build_value(UmiDataSchemaIndex *item, const char *index_id, const char *table_id, const char *key_expression, bool unique) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->index_id,sizeof(item->index_id),index_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->table_id,sizeof(item->table_id),table_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->key_expression,sizeof(item->key_expression),key_expression); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; item->unique=unique; item->covering=false;
    return umi_data_schema_index_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_schema_index_init, UmiDataSchemaIndex, item,
    (UmiDataSchemaIndex *item, const char *index_id, const char *table_id, const char *key_expression, bool unique),
    umi_data_schema_index_init_build_value(candidate, index_id, table_id, key_expression, unique))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_index_validate(const UmiDataSchemaIndex *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->index_id, '\0', sizeof(item->index_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->table_id, '\0', sizeof(item->table_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->key_expression, '\0', sizeof(item->key_expression)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->index_id[0] != '\0' && item->table_id[0] != '\0' && item->key_expression[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSchemaIndexArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfe8fe1935304ea42);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIndex *)0)->index_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIndex *)0)->table_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIndex *)0)->key_expression)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSchemaIndexArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSchemaIndex *)0)->index_id) - 1U +
        8U + sizeof(((UmiDataSchemaIndex *)0)->table_id) - 1U +
        8U + sizeof(((UmiDataSchemaIndex *)0)->key_expression) - 1U +
        8U +
        8U;
}
static void UmiDataSchemaIndexArchiveWrite(UmiArchiveWriter *writer, const UmiDataSchemaIndex *value)
{
    UmiArchiveWriteText(writer, value->index_id, sizeof(value->index_id));
    UmiArchiveWriteText(writer, value->table_id, sizeof(value->table_id));
    UmiArchiveWriteText(writer, value->key_expression, sizeof(value->key_expression));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->unique);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->covering);
}
static void UmiDataSchemaIndexArchiveRead(UmiArchiveReader *reader, UmiDataSchemaIndex *value)
{
    UmiArchiveReadText(reader, value->index_id, sizeof(value->index_id));
    UmiArchiveReadText(reader, value->table_id, sizeof(value->table_id));
    UmiArchiveReadText(reader, value->key_expression, sizeof(value->key_expression));
    value->unique = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->covering = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataSchemaIndexArchiveValidate(const UmiDataSchemaIndex *value)
{
    return umi_data_schema_index_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_schema_index_archive_encode, umi_data_schema_index_archive_decode,
    UmiDataSchemaIndex, UmiDataSchemaIndexArchiveSchema, UmiDataSchemaIndexArchiveBound, UmiDataSchemaIndexArchiveWrite, UmiDataSchemaIndexArchiveRead, UmiDataSchemaIndexArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_schema_index_archive_decode_if_unchanged, umi_data_schema_index_archive_encode,
    UmiDataSchemaIndex, UmiDataSchemaIndexArchiveSchema, UmiDataSchemaIndexArchiveBound, UmiDataSchemaIndexArchiveRead, UmiDataSchemaIndexArchiveValidate)
