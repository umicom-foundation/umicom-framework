/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/statement_plan.c
 *
 * PURPOSE:
 *   Describe a prepared statement contract and its query/schema fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/statement_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_statement_plan_init(UmiDataStatementPlan *item, const char *statement_id, uint64_t query_fingerprint, uint64_t schema_fingerprint, size_t parameter_count, bool read_only) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->statement_id,sizeof(item->statement_id),statement_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->query_fingerprint=query_fingerprint;item->schema_fingerprint=schema_fingerprint;item->parameter_count=parameter_count;item->read_only=read_only;
    return umi_data_statement_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_statement_plan_validate(const UmiDataStatementPlan *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->statement_id, '\0', sizeof(item->statement_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->statement_id[0] != '\0' && item->query_fingerprint != 0U && item->schema_fingerprint != 0U && item->parameter_count <= UMI_DATA_ENTERPRISE_MAX_ITEMS)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataStatementPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc8494aa4ff73a4fa);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataStatementPlan *)0)->statement_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataStatementPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataStatementPlan *)0)->statement_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataStatementPlanArchiveWrite(UmiArchiveWriter *writer, const UmiDataStatementPlan *value)
{
    UmiArchiveWriteText(writer, value->statement_id, sizeof(value->statement_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->query_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->schema_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parameter_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->read_only);
}
static void UmiDataStatementPlanArchiveRead(UmiArchiveReader *reader, UmiDataStatementPlan *value)
{
    UmiArchiveReadText(reader, value->statement_id, sizeof(value->statement_id));
    value->query_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->schema_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->parameter_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->read_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataStatementPlanArchiveValidate(const UmiDataStatementPlan *value)
{
    return umi_data_statement_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_statement_plan_archive_encode, umi_data_statement_plan_archive_decode,
    UmiDataStatementPlan, UmiDataStatementPlanArchiveSchema, UmiDataStatementPlanArchiveBound, UmiDataStatementPlanArchiveWrite, UmiDataStatementPlanArchiveRead, UmiDataStatementPlanArchiveValidate)
