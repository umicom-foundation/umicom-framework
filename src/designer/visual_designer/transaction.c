/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/transaction.c
 *
 * PURPOSE:
 *   Track atomic designer transactions and mutation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/transaction.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise visual designer transaction from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_transaction_init(UmiRadDesignerTransaction *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->transaction_id, sizeof item->transaction_id, "transaction");
    return UMI_STATUS_OK;
}
/* Check that visual designer transaction satisfies its contract before another service relies on it. */
int umi_rad_transaction_is_valid(const UmiRadDesignerTransaction *item){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->transaction_id, '\0', sizeof(item->transaction_id)) == NULL) return 0;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->transaction_id);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRadDesignerTransactionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1d8ee9d15c58943a);
    schema = (schema ^ (uint64_t)sizeof(((UmiRadDesignerTransaction *)0)->transaction_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRadDesignerTransactionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRadDesignerTransaction *)0)->transaction_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiRadDesignerTransactionArchiveWrite(UmiArchiveWriter *writer, const UmiRadDesignerTransaction *value)
{
    UmiArchiveWriteText(writer, value->transaction_id, sizeof(value->transaction_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->mutation_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiRadDesignerTransactionArchiveRead(UmiArchiveReader *reader, UmiRadDesignerTransaction *value)
{
    UmiArchiveReadText(reader, value->transaction_id, sizeof(value->transaction_id));
    value->state = (UmiRadTransactionState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->mutation_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiRadDesignerTransactionArchiveValidate(const UmiRadDesignerTransaction *value)
{
    return umi_rad_transaction_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_rad_transaction_archive_encode, umi_rad_transaction_archive_decode,
    UmiRadDesignerTransaction, UmiRadDesignerTransactionArchiveSchema, UmiRadDesignerTransactionArchiveBound, UmiRadDesignerTransactionArchiveWrite, UmiRadDesignerTransactionArchiveRead, UmiRadDesignerTransactionArchiveValidate)
