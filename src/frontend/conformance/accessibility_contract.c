/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/accessibility_contract.c
 *
 * PURPOSE:
 *   required semantic accessibility roles, names, states and keyboard affordances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/accessibility_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc accessibility contract satisfies its contract before another service
 * relies on it.
 */
bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->named && item->keyboard_reachable && item->state_exposed;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcAccessibilityContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4c284d16d4cef558);

    return schema;
}
static size_t UmiFcAccessibilityContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiFcAccessibilityContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcAccessibilityContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_roles);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->named);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->keyboard_reachable);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->state_exposed);
}
static void UmiFcAccessibilityContractArchiveRead(UmiArchiveReader *reader, UmiFcAccessibilityContract *value)
{
    value->required_roles = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->named = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->keyboard_reachable = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->state_exposed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcAccessibilityContractArchiveValidate(const UmiFcAccessibilityContract *value)
{
    return umi_fc_accessibility_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_accessibility_contract_archive_encode, umi_fc_accessibility_contract_archive_decode,
    UmiFcAccessibilityContract, UmiFcAccessibilityContractArchiveSchema, UmiFcAccessibilityContractArchiveBound, UmiFcAccessibilityContractArchiveWrite, UmiFcAccessibilityContractArchiveRead, UmiFcAccessibilityContractArchiveValidate)
