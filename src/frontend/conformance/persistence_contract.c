/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/persistence_contract.c
 *
 * PURPOSE:
 *   layout, focus, panel, geometry and context state persistence requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/persistence_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc persistence contract satisfies its contract before another service relies
 * on it.
 */
bool umi_fc_persistence_contract_validate(const UmiFcPersistenceContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_fields!=0U && item->schema_version>0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcPersistenceContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc9e673bc1ff26468);

    return schema;
}
static size_t UmiFcPersistenceContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcPersistenceContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcPersistenceContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_fields);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->schema_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->forward_readable);
}
static void UmiFcPersistenceContractArchiveRead(UmiArchiveReader *reader, UmiFcPersistenceContract *value)
{
    value->required_fields = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->schema_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->forward_readable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcPersistenceContractArchiveValidate(const UmiFcPersistenceContract *value)
{
    return umi_fc_persistence_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_persistence_contract_archive_encode, umi_fc_persistence_contract_archive_decode,
    UmiFcPersistenceContract, UmiFcPersistenceContractArchiveSchema, UmiFcPersistenceContractArchiveBound, UmiFcPersistenceContractArchiveWrite, UmiFcPersistenceContractArchiveRead, UmiFcPersistenceContractArchiveValidate)
