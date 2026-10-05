/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/bank_product.c
 *
 * PURPOSE:
 *   Implement describe reusable banking product templates independent of channel applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking bank product from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_bank_product_init(UmiBankingBankProduct *value,
    const char *id,
    const char *name,
    UmiBankingProductKind kind,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->kind=kind;
    value->active=active;
    return umi_banking_bank_product_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking bank product satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_bank_product_valid(const UmiBankingBankProduct *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;

    return value!=NULL && (value->name[0]!='\0' && value->kind>=UMI_BANKING_PRODUCT_DEPOSIT && value->kind<=UMI_BANKING_PRODUCT_OVERDRAFT);
}

/*
 * Provide the banking bank product available operation used by this module and its client
 * applications.
 */
bool umi_banking_bank_product_available(const UmiBankingBankProduct *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->active;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingBankProductArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdd4a3cf64d4ce58a);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingBankProduct *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingBankProduct *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingBankProductArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingBankProduct *)0)->id.value) - 1U +
        8U + sizeof(((UmiBankingBankProduct *)0)->name) - 1U +
        8U +
        8U;
}
static void UmiBankingBankProductArchiveWrite(UmiArchiveWriter *writer, const UmiBankingBankProduct *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiBankingBankProductArchiveRead(UmiArchiveReader *reader, UmiBankingBankProduct *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->kind = (UmiBankingProductKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiBankingBankProductArchiveValidate(const UmiBankingBankProduct *value)
{
    return umi_banking_bank_product_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_bank_product_archive_encode, umi_banking_bank_product_archive_decode,
    UmiBankingBankProduct, UmiBankingBankProductArchiveSchema, UmiBankingBankProductArchiveBound, UmiBankingBankProductArchiveWrite, UmiBankingBankProductArchiveRead, UmiBankingBankProductArchiveValidate)
