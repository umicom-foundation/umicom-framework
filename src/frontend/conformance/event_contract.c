/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/event_contract.c
 *
 * PURPOSE:
 *   semantic user-event support requirements independent of native toolkit event classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_contract.h"
#include "../../base/value_archive_internal.h"

/* Check that fc event contract satisfies its contract before another service relies on it. */
bool umi_fc_event_contract_validate(const UmiFcEventContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_families!=0U && item->ordered;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcEventContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x021a9657942a868f);

    return schema;
}
static size_t UmiFcEventContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcEventContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcEventContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_families);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ordered);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cancellable);
}
static void UmiFcEventContractArchiveRead(UmiArchiveReader *reader, UmiFcEventContract *value)
{
    value->required_families = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->ordered = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->cancellable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcEventContractArchiveValidate(const UmiFcEventContract *value)
{
    return umi_fc_event_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_event_contract_archive_encode, umi_fc_event_contract_archive_decode,
    UmiFcEventContract, UmiFcEventContractArchiveSchema, UmiFcEventContractArchiveBound, UmiFcEventContractArchiveWrite, UmiFcEventContractArchiveRead, UmiFcEventContractArchiveValidate)
