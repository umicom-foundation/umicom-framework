/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_column.c
 *
 * PURPOSE:
 *   Describe portable column metadata used by schema diffing, ORM mapping and migrations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_column.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_column_init(UmiDataSchemaColumn *item, const char *column_id, const char *name, UmiDataValueKind kind, uint32_t ordinal, bool nullable) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->column_id,sizeof(item->column_id),column_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    s=umi_data_enterprise_copy_text(item->name,sizeof(item->name),name); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;
    item->kind=kind; item->ordinal=ordinal; item->nullable=nullable; item->generated=false;
    return umi_data_schema_column_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_column_validate(const UmiDataSchemaColumn *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->column_id, '\0', sizeof(item->column_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->column_id[0] != '\0' && item->name[0] != '\0' && item->kind >= UMI_DATA_VALUE_INTEGER && item->kind <= UMI_DATA_VALUE_DECIMAL)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSchemaColumnArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x708285d5511494c6);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaColumn *)0)->column_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaColumn *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSchemaColumnArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSchemaColumn *)0)->column_id) - 1U +
        8U + sizeof(((UmiDataSchemaColumn *)0)->name) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataSchemaColumnArchiveWrite(UmiArchiveWriter *writer, const UmiDataSchemaColumn *value)
{
    UmiArchiveWriteText(writer, value->column_id, sizeof(value->column_id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ordinal);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->nullable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generated);
}
static void UmiDataSchemaColumnArchiveRead(UmiArchiveReader *reader, UmiDataSchemaColumn *value)
{
    UmiArchiveReadText(reader, value->column_id, sizeof(value->column_id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->kind = (UmiDataValueKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->ordinal = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->nullable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->generated = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataSchemaColumnArchiveValidate(const UmiDataSchemaColumn *value)
{
    return umi_data_schema_column_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_schema_column_archive_encode, umi_data_schema_column_archive_decode,
    UmiDataSchemaColumn, UmiDataSchemaColumnArchiveSchema, UmiDataSchemaColumnArchiveBound, UmiDataSchemaColumnArchiveWrite, UmiDataSchemaColumnArchiveRead, UmiDataSchemaColumnArchiveValidate)
