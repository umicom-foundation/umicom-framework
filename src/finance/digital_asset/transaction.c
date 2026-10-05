/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/transaction.c
 *
 * PURPOSE:
 *   Implement a provider-neutral on-chain transaction and confirmation evidence.
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

#include "umicom/finance/digital_asset/transaction.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_transaction_init(UmiDigitalAssetTransaction *value, const char *id, const char *network_id, const char *from_address, const char *to_address, int64_t units, int32_t scale, const char *asset_symbol)
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
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->from_address, sizeof value->from_address, from_address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->to_address, sizeof value->to_address, to_address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->amount.asset_symbol, sizeof value->amount.asset_symbol, asset_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->amount.units = units;
    value->amount.scale = scale;
    value->state = UMI_DIGITAL_TX_CREATED;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_transaction_valid(const UmiDigitalAssetTransaction *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;
    if (memchr(value->from_address, '\0', sizeof(value->from_address)) == NULL) return 0;
    if (memchr(value->to_address, '\0', sizeof(value->to_address)) == NULL) return 0;
    if (memchr(value->amount.asset_symbol, '\0', sizeof(value->amount.asset_symbol)) == NULL) return 0;
    if (memchr(value->transaction_hash, '\0', sizeof(value->transaction_hash)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->to_address) && value->amount.units > 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalAssetTransactionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x61dfe48b6e563bc1);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->network_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->from_address)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->to_address)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->amount.asset_symbol)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetTransaction *)0)->transaction_hash)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalAssetTransactionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->network_id.value) - 1U +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->from_address) - 1U +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->to_address) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->amount.asset_symbol) - 1U +
        8U + sizeof(((UmiDigitalAssetTransaction *)0)->transaction_hash) - 1U +
        8U +
        8U;
}
static void UmiDigitalAssetTransactionArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalAssetTransaction *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteText(writer, value->from_address, sizeof(value->from_address));
    UmiArchiveWriteText(writer, value->to_address, sizeof(value->to_address));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->amount.scale);
    UmiArchiveWriteText(writer, value->amount.asset_symbol, sizeof(value->amount.asset_symbol));
    UmiArchiveWriteText(writer, value->transaction_hash, sizeof(value->transaction_hash));
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->confirmations);
}
static void UmiDigitalAssetTransactionArchiveRead(UmiArchiveReader *reader, UmiDigitalAssetTransaction *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveReadText(reader, value->from_address, sizeof(value->from_address));
    UmiArchiveReadText(reader, value->to_address, sizeof(value->to_address));
    value->amount.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->amount.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->amount.asset_symbol, sizeof(value->amount.asset_symbol));
    UmiArchiveReadText(reader, value->transaction_hash, sizeof(value->transaction_hash));
    value->state = (UmiDigitalTransactionState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->confirmations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiDigitalAssetTransactionArchiveValidate(const UmiDigitalAssetTransaction *value)
{
    return umi_digital_asset_transaction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_transaction_archive_encode, umi_digital_asset_transaction_archive_decode,
    UmiDigitalAssetTransaction, UmiDigitalAssetTransactionArchiveSchema, UmiDigitalAssetTransactionArchiveBound, UmiDigitalAssetTransactionArchiveWrite, UmiDigitalAssetTransactionArchiveRead, UmiDigitalAssetTransactionArchiveValidate)
