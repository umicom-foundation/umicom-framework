/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/transfer_instruction.c
 *
 * PURPOSE:
 *   Implement a governed digital-asset transfer between custody or external addresses.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/transfer_instruction.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_transfer_instruction_init(UmiDigitalTransferInstruction *value, const char *id, const char *source_account_id, const char *destination_address, int64_t units, int32_t scale, const char *asset_symbol)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units <= 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->source_account_id.value, sizeof value->source_account_id.value, source_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->destination_address, sizeof value->destination_address, destination_address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->amount.asset_symbol, sizeof value->amount.asset_symbol, asset_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->amount.units = units;
    value->amount.scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_transfer_instruction_valid(const UmiDigitalTransferInstruction *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->source_account_id.value, '\0', sizeof(value->source_account_id.value)) == NULL) return 0;
    if (memchr(value->destination_address, '\0', sizeof(value->destination_address)) == NULL) return 0;
    if (memchr(value->amount.asset_symbol, '\0', sizeof(value->amount.asset_symbol)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->source_account_id.value) && umi_digital_asset_text_valid(value->destination_address) && value->amount.units > 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalTransferInstructionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb9d437b67feb52b5);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTransferInstruction *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTransferInstruction *)0)->source_account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTransferInstruction *)0)->destination_address)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTransferInstruction *)0)->amount.asset_symbol)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalTransferInstructionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalTransferInstruction *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalTransferInstruction *)0)->source_account_id.value) - 1U +
        8U + sizeof(((UmiDigitalTransferInstruction *)0)->destination_address) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiDigitalTransferInstruction *)0)->amount.asset_symbol) - 1U +
        8U +
        8U;
}
static void UmiDigitalTransferInstructionArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalTransferInstruction *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->source_account_id.value, sizeof(value->source_account_id.value));
    UmiArchiveWriteText(writer, value->destination_address, sizeof(value->destination_address));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->amount.scale);
    UmiArchiveWriteText(writer, value->amount.asset_symbol, sizeof(value->amount.asset_symbol));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->approved);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->submitted);
}
static void UmiDigitalTransferInstructionArchiveRead(UmiArchiveReader *reader, UmiDigitalTransferInstruction *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->source_account_id.value, sizeof(value->source_account_id.value));
    UmiArchiveReadText(reader, value->destination_address, sizeof(value->destination_address));
    value->amount.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->amount.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->amount.asset_symbol, sizeof(value->amount.asset_symbol));
    value->approved = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->submitted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalTransferInstructionArchiveValidate(const UmiDigitalTransferInstruction *value)
{
    return umi_digital_asset_transfer_instruction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_transfer_instruction_archive_encode, umi_digital_asset_transfer_instruction_archive_decode,
    UmiDigitalTransferInstruction, UmiDigitalTransferInstructionArchiveSchema, UmiDigitalTransferInstructionArchiveBound, UmiDigitalTransferInstructionArchiveWrite, UmiDigitalTransferInstructionArchiveRead, UmiDigitalTransferInstructionArchiveValidate)
