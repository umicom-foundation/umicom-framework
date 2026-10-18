/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_identifier.c
 *
 * PURPOSE:
 *   Represent a qualified schema object identifier without binding to a specific SQL engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/schema_identifier.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_schema_identifier_init(UmiDataSchemaIdentifier *item, const char *catalog, const char *schema, const char *name) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->catalog,sizeof(item->catalog),catalog); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (schema != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->schema,sizeof(item->schema),schema); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    return umi_data_enterprise_copy_text(item->name,sizeof(item->name),name);
    return umi_data_schema_identifier_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_schema_identifier_init_build_value(UmiDataSchemaIdentifier *item, const char *catalog, const char *schema, const char *name) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalog != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->catalog,sizeof(item->catalog),catalog); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (schema != NULL) { UmiStatus s=umi_data_enterprise_copy_text(item->schema,sizeof(item->schema),schema); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; }
    return umi_data_enterprise_copy_text(item->name,sizeof(item->name),name);
    return umi_data_schema_identifier_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_schema_identifier_init, UmiDataSchemaIdentifier, item,
    (UmiDataSchemaIdentifier *item, const char *catalog, const char *schema, const char *name),
    umi_data_schema_identifier_init_build_value(candidate, catalog, schema, name))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_identifier_validate(const UmiDataSchemaIdentifier *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->catalog, '\0', sizeof(item->catalog)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->schema, '\0', sizeof(item->schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->name, '\0', sizeof(item->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->name[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSchemaIdentifierArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2ff626bb3a9e9aef);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIdentifier *)0)->catalog)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIdentifier *)0)->schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSchemaIdentifier *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSchemaIdentifierArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSchemaIdentifier *)0)->catalog) - 1U +
        8U + sizeof(((UmiDataSchemaIdentifier *)0)->schema) - 1U +
        8U + sizeof(((UmiDataSchemaIdentifier *)0)->name) - 1U;
}
static void UmiDataSchemaIdentifierArchiveWrite(UmiArchiveWriter *writer, const UmiDataSchemaIdentifier *value)
{
    UmiArchiveWriteText(writer, value->catalog, sizeof(value->catalog));
    UmiArchiveWriteText(writer, value->schema, sizeof(value->schema));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
}
static void UmiDataSchemaIdentifierArchiveRead(UmiArchiveReader *reader, UmiDataSchemaIdentifier *value)
{
    UmiArchiveReadText(reader, value->catalog, sizeof(value->catalog));
    UmiArchiveReadText(reader, value->schema, sizeof(value->schema));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
}
static UmiStatus UmiDataSchemaIdentifierArchiveValidate(const UmiDataSchemaIdentifier *value)
{
    return umi_data_schema_identifier_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_schema_identifier_archive_encode, umi_data_schema_identifier_archive_decode,
    UmiDataSchemaIdentifier, UmiDataSchemaIdentifierArchiveSchema, UmiDataSchemaIdentifierArchiveBound, UmiDataSchemaIdentifierArchiveWrite, UmiDataSchemaIdentifierArchiveRead, UmiDataSchemaIdentifierArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_data_schema_identifier_archive_decode_with_workspace, UmiDataSchemaIdentifier,
    UmiDataSchemaIdentifierArchiveSchema, UmiDataSchemaIdentifierArchiveBound, UmiDataSchemaIdentifierArchiveRead, UmiDataSchemaIdentifierArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_schema_identifier_archive_decode_if_unchanged, umi_data_schema_identifier_archive_encode,
    UmiDataSchemaIdentifier, UmiDataSchemaIdentifierArchiveSchema, UmiDataSchemaIdentifierArchiveBound, UmiDataSchemaIdentifierArchiveRead, UmiDataSchemaIdentifierArchiveValidate)
