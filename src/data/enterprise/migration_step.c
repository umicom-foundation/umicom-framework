/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_step.c
 *
 * PURPOSE:
 *   Describe one reversible or irreversible schema/data migration operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_step.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_migration_step_init(UmiDataMigrationStep *item, const char *step_id, const char *description, uint32_t ordinal, bool reversible, bool destructive) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->step_id,sizeof(item->step_id),step_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->description,sizeof(item->description),description);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->ordinal=ordinal;item->reversible=reversible;item->destructive=destructive;
    return umi_data_migration_step_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_migration_step_validate(const UmiDataMigrationStep *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->step_id, '\0', sizeof(item->step_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->description, '\0', sizeof(item->description)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->step_id[0] != '\0' && item->description[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataMigrationStepArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2e95c416293acf66);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataMigrationStep *)0)->step_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataMigrationStep *)0)->description)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataMigrationStepArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataMigrationStep *)0)->step_id) - 1U +
        8U + sizeof(((UmiDataMigrationStep *)0)->description) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDataMigrationStepArchiveWrite(UmiArchiveWriter *writer, const UmiDataMigrationStep *value)
{
    UmiArchiveWriteText(writer, value->step_id, sizeof(value->step_id));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ordinal);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reversible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->destructive);
}
static void UmiDataMigrationStepArchiveRead(UmiArchiveReader *reader, UmiDataMigrationStep *value)
{
    UmiArchiveReadText(reader, value->step_id, sizeof(value->step_id));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    value->ordinal = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->reversible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->destructive = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataMigrationStepArchiveValidate(const UmiDataMigrationStep *value)
{
    return umi_data_migration_step_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_migration_step_archive_encode, umi_data_migration_step_archive_decode,
    UmiDataMigrationStep, UmiDataMigrationStepArchiveSchema, UmiDataMigrationStepArchiveBound, UmiDataMigrationStepArchiveWrite, UmiDataMigrationStepArchiveRead, UmiDataMigrationStepArchiveValidate)
