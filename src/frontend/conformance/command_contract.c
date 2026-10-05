/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/command_contract.c
 *
 * PURPOSE:
 *   stable command exposure expectations independent of frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/command_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc command contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_command_contract_validate(const UmiFcCommandContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_commands==0U || (item->command_fingerprint!=0U && item->all_have_stable_ids);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcCommandContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x390df1ba4350f316);

    return schema;
}
static size_t UmiFcCommandContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcCommandContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcCommandContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_commands);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->command_fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->all_have_stable_ids);
}
static void UmiFcCommandContractArchiveRead(UmiArchiveReader *reader, UmiFcCommandContract *value)
{
    value->required_commands = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->command_fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->all_have_stable_ids = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcCommandContractArchiveValidate(const UmiFcCommandContract *value)
{
    return umi_fc_command_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_command_contract_archive_encode, umi_fc_command_contract_archive_decode,
    UmiFcCommandContract, UmiFcCommandContractArchiveSchema, UmiFcCommandContractArchiveBound, UmiFcCommandContractArchiveWrite, UmiFcCommandContractArchiveRead, UmiFcCommandContractArchiveValidate)
