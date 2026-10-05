/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/docking_contract.c
 *
 * PURPOSE:
 *   dock zones, floating, auto-hide and split/tab workstation requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc docking contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_docking_contract_validate(const UmiFcDockingContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_features!=0U && item->allowed_zones!=0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcDockingContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb696d4388e802636);

    return schema;
}
static size_t UmiFcDockingContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcDockingContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcDockingContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_features);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allowed_zones);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->responsive_fallback);
}
static void UmiFcDockingContractArchiveRead(UmiArchiveReader *reader, UmiFcDockingContract *value)
{
    value->required_features = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->allowed_zones = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->responsive_fallback = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcDockingContractArchiveValidate(const UmiFcDockingContract *value)
{
    return umi_fc_docking_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_docking_contract_archive_encode, umi_fc_docking_contract_archive_decode,
    UmiFcDockingContract, UmiFcDockingContractArchiveSchema, UmiFcDockingContractArchiveBound, UmiFcDockingContractArchiveWrite, UmiFcDockingContractArchiveRead, UmiFcDockingContractArchiveValidate)
