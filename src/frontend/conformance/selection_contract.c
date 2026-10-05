/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/selection_contract.c
 *
 * PURPOSE:
 *   single, multiple and range selection semantics for list, tree, grid and editor surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/selection_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc selection contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_selection_contract_validate(const UmiFcSelectionContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_modes!=0U && item->preserve_on_refresh;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcSelectionContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x131d642c693ccb05);

    return schema;
}
static size_t UmiFcSelectionContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcSelectionContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcSelectionContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_modes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->keyboard_extend);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->preserve_on_refresh);
}
static void UmiFcSelectionContractArchiveRead(UmiArchiveReader *reader, UmiFcSelectionContract *value)
{
    value->required_modes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->keyboard_extend = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->preserve_on_refresh = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcSelectionContractArchiveValidate(const UmiFcSelectionContract *value)
{
    return umi_fc_selection_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_selection_contract_archive_encode, umi_fc_selection_contract_archive_decode,
    UmiFcSelectionContract, UmiFcSelectionContractArchiveSchema, UmiFcSelectionContractArchiveBound, UmiFcSelectionContractArchiveWrite, UmiFcSelectionContractArchiveRead, UmiFcSelectionContractArchiveValidate)
