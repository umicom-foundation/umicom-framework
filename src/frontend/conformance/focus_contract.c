/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/focus_contract.c
 *
 * PURPOSE:
 *   focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/focus_contract.h"
#include "../../base/value_archive_internal.h"

/* Check that fc focus contract satisfies its contract before another service relies on it. */
bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->traversal_count>=item->focusable_count;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcFocusContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb4185ba4d9893dfa);

    return schema;
}
static size_t UmiFcFocusContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcFocusContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcFocusContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->focusable_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->traversal_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->modal_trap_required);
}
static void UmiFcFocusContractArchiveRead(UmiArchiveReader *reader, UmiFcFocusContract *value)
{
    value->focusable_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->traversal_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->modal_trap_required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcFocusContractArchiveValidate(const UmiFcFocusContract *value)
{
    return umi_fc_focus_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_focus_contract_archive_encode, umi_fc_focus_contract_archive_decode,
    UmiFcFocusContract, UmiFcFocusContractArchiveSchema, UmiFcFocusContractArchiveBound, UmiFcFocusContractArchiveWrite, UmiFcFocusContractArchiveRead, UmiFcFocusContractArchiveValidate)
