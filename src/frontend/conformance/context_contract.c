/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/context_contract.c
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc context contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_context_contract_validate(const UmiFcContextContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_types!=0U && item->accessible_label;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcContextContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x096a49fd30921359);

    return schema;
}
static size_t UmiFcContextContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcContextContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcContextContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_types);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->bidirectional);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accessible_label);
}
static void UmiFcContextContractArchiveRead(UmiArchiveReader *reader, UmiFcContextContract *value)
{
    value->required_types = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->bidirectional = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->accessible_label = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcContextContractArchiveValidate(const UmiFcContextContract *value)
{
    return umi_fc_context_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_context_contract_archive_encode, umi_fc_context_contract_archive_decode,
    UmiFcContextContract, UmiFcContextContractArchiveSchema, UmiFcContextContractArchiveBound, UmiFcContextContractArchiveWrite, UmiFcContextContractArchiveRead, UmiFcContextContractArchiveValidate)
