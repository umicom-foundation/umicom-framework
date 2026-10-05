/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/keyboard_contract.c
 *
 * PURPOSE:
 *   required command and navigation keyboard coverage for workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc keyboard contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_keyboard_contract_validate(const UmiFcKeyboardContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->command_count>0U && item->navigation_count>0U && item->shortcuts_documented;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcKeyboardContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1c605222afc42c32);

    return schema;
}
static size_t UmiFcKeyboardContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcKeyboardContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcKeyboardContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->command_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->navigation_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->shortcuts_documented);
}
static void UmiFcKeyboardContractArchiveRead(UmiArchiveReader *reader, UmiFcKeyboardContract *value)
{
    value->command_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->navigation_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->shortcuts_documented = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcKeyboardContractArchiveValidate(const UmiFcKeyboardContract *value)
{
    return umi_fc_keyboard_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_keyboard_contract_archive_encode, umi_fc_keyboard_contract_archive_decode,
    UmiFcKeyboardContract, UmiFcKeyboardContractArchiveSchema, UmiFcKeyboardContractArchiveBound, UmiFcKeyboardContractArchiveWrite, UmiFcKeyboardContractArchiveRead, UmiFcKeyboardContractArchiveValidate)
