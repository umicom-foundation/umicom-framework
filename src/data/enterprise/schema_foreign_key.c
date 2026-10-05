/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_foreign_key.c
 *
 * PURPOSE:
 *   Describe referential constraints in a backend-neutral form for migration ordering and ORM relations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_foreign_key.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_foreign_key_init(UmiDataSchemaForeignKey *item, const char *constraint_id, const char *source_table, const char *source_column, const char *target_table, const char *target_column) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->constraint_id,sizeof(item->constraint_id),constraint_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->source_table,sizeof(item->source_table),source_table); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->source_column,sizeof(item->source_column),source_column); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->target_table,sizeof(item->target_table),target_table); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->target_column,sizeof(item->target_column),target_column); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s; item->cascade_delete=false;
    return umi_data_schema_foreign_key_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_foreign_key_validate(const UmiDataSchemaForeignKey *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->constraint_id, '\0', sizeof(item->constraint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_table, '\0', sizeof(item->source_table)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->target_table, '\0', sizeof(item->target_table)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->source_column, '\0', sizeof(item->source_column)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->target_column, '\0', sizeof(item->target_column)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->constraint_id[0] != '\0' && item->source_table[0] != '\0' && item->target_table[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSchemaForeignKeyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf897fa633bcdc931);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaForeignKey *)0)->constraint_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaForeignKey *)0)->source_table)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaForeignKey *)0)->target_table)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaForeignKey *)0)->source_column)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaForeignKey *)0)->target_column)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSchemaForeignKeyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSchemaForeignKey *)0)->constraint_id) - 1U +
        8U + sizeof(((UmiDataSchemaForeignKey *)0)->source_table) - 1U +
        8U + sizeof(((UmiDataSchemaForeignKey *)0)->target_table) - 1U +
        8U + sizeof(((UmiDataSchemaForeignKey *)0)->source_column) - 1U +
        8U + sizeof(((UmiDataSchemaForeignKey *)0)->target_column) - 1U +
        8U;
}
static void UmiDataSchemaForeignKeyArchiveWrite(UmiArchiveWriter *writer, const UmiDataSchemaForeignKey *value)
{
    UmiArchiveWriteText(writer, value->constraint_id, sizeof(value->constraint_id));
    UmiArchiveWriteText(writer, value->source_table, sizeof(value->source_table));
    UmiArchiveWriteText(writer, value->target_table, sizeof(value->target_table));
    UmiArchiveWriteText(writer, value->source_column, sizeof(value->source_column));
    UmiArchiveWriteText(writer, value->target_column, sizeof(value->target_column));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cascade_delete);
}
static void UmiDataSchemaForeignKeyArchiveRead(UmiArchiveReader *reader, UmiDataSchemaForeignKey *value)
{
    UmiArchiveReadText(reader, value->constraint_id, sizeof(value->constraint_id));
    UmiArchiveReadText(reader, value->source_table, sizeof(value->source_table));
    UmiArchiveReadText(reader, value->target_table, sizeof(value->target_table));
    UmiArchiveReadText(reader, value->source_column, sizeof(value->source_column));
    UmiArchiveReadText(reader, value->target_column, sizeof(value->target_column));
    value->cascade_delete = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataSchemaForeignKeyArchiveValidate(const UmiDataSchemaForeignKey *value)
{
    return umi_data_schema_foreign_key_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_schema_foreign_key_archive_encode, umi_data_schema_foreign_key_archive_decode,
    UmiDataSchemaForeignKey, UmiDataSchemaForeignKeyArchiveSchema, UmiDataSchemaForeignKeyArchiveBound, UmiDataSchemaForeignKeyArchiveWrite, UmiDataSchemaForeignKeyArchiveRead, UmiDataSchemaForeignKeyArchiveValidate)
