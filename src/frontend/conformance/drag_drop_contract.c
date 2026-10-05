/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/drag_drop_contract.c
 *
 * PURPOSE:
 *   semantic drag/drop operation, keyboard alternative and docking affordance requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/drag_drop_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that fc drag drop contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_drag_drop_contract_validate(const UmiFcDragDropContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_ops!=0U && item->keyboard_alternative;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFcDragDropContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x90d29b2ee5cfdb11);

    return schema;
}
static size_t UmiFcDragDropContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiFcDragDropContractArchiveWrite(UmiArchiveWriter *writer, const UmiFcDragDropContract *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_ops);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->keyboard_alternative);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visual_preview);
}
static void UmiFcDragDropContractArchiveRead(UmiArchiveReader *reader, UmiFcDragDropContract *value)
{
    value->required_ops = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->keyboard_alternative = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->visual_preview = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiFcDragDropContractArchiveValidate(const UmiFcDragDropContract *value)
{
    return umi_fc_drag_drop_contract_validate(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fc_drag_drop_contract_archive_encode, umi_fc_drag_drop_contract_archive_decode,
    UmiFcDragDropContract, UmiFcDragDropContractArchiveSchema, UmiFcDragDropContractArchiveBound, UmiFcDragDropContractArchiveWrite, UmiFcDragDropContractArchiveRead, UmiFcDragDropContractArchiveValidate)
