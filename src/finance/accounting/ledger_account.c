/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_account.c
 *
 * PURPOSE:
 *   Implement represent general-ledger accounts, normal side and posting eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/finance/accounting/ledger_account.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Retain the former implementation for review. The replacement validates
 * complete restored values and bounded containers before lookup or eligibility,
 * and stages initialization before publication to preserve caller data. */
#if 0
/*
 * Initialise accounting ledger account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id,
    const char *name,
    UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side,
    bool posting_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->account_class=account_class;
    value->normal_side=normal_side;
    value->posting_allowed=posting_allowed;
    return umi_accounting_ledger_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting ledger account satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;

    return value!=NULL && (value->name[0]!='\0' && value->account_class>=UMI_ACCOUNTING_ASSET && value->account_class<=UMI_ACCOUNTING_EXPENSE && (value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT||value->normal_side==UMI_ACCOUNTING_NORMAL_CREDIT));
}

/*
 * Provide the accounting ledger account postable operation used by this module and its
 * client applications.
 */
bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->posting_allowed;
}


#endif

/* Account identity and posting eligibility share one validator. Restored values
 * must satisfy it before an application treats an account as usable. */
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id, const char *name, UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side, bool posting_allowed)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAccountingLedgerAccount next = {0};
    UmiStatus status = umi_accounting_id_assign(&next.id, id);
    if (status == UMI_STATUS_OK) {
        status = umi_financial_core_copy(next.name, sizeof next.name, name);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    next.account_class = account_class;
    next.normal_side = normal_side;
    next.posting_allowed = posting_allowed;
    if (!umi_accounting_ledger_account_valid(&next)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *value = next;
    return UMI_STATUS_OK;
}

bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value)
{
    if (value == NULL || !umi_financial_id_is_valid(&value->id) ||
        memchr(value->name, '\0', sizeof value->name) == NULL || value->name[0] == '\0' ||
        (value->normal_side != UMI_ACCOUNTING_NORMAL_DEBIT &&
         value->normal_side != UMI_ACCOUNTING_NORMAL_CREDIT)) {
        return false;
    }
    switch (value->account_class) {
    case UMI_ACCOUNTING_ASSET:
    case UMI_ACCOUNTING_LIABILITY:
    case UMI_ACCOUNTING_EQUITY:
    case UMI_ACCOUNTING_REVENUE:
    case UMI_ACCOUNTING_EXPENSE:
        return true;
    default:
        return false;
    }
}

bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value)
{
    return umi_accounting_ledger_account_valid(value) && value->posting_allowed;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAccountingLedgerAccountArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc89ff1606f7d15d3);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingLedgerAccount *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingLedgerAccount *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAccountingLedgerAccountArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAccountingLedgerAccount *)0)->id.value) - 1U +
        8U + sizeof(((UmiAccountingLedgerAccount *)0)->name) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAccountingLedgerAccountArchiveWrite(UmiArchiveWriter *writer, const UmiAccountingLedgerAccount *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->account_class);
    UmiArchiveWriteSigned(writer, (int64_t)value->normal_side);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->posting_allowed);
}
static void UmiAccountingLedgerAccountArchiveRead(UmiArchiveReader *reader, UmiAccountingLedgerAccount *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->account_class = (UmiAccountingAccountClass)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->normal_side = (UmiAccountingNormalSide)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->posting_allowed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAccountingLedgerAccountArchiveValidate(const UmiAccountingLedgerAccount *value)
{
    return umi_accounting_ledger_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_accounting_ledger_account_archive_encode, umi_accounting_ledger_account_archive_decode,
    UmiAccountingLedgerAccount, UmiAccountingLedgerAccountArchiveSchema, UmiAccountingLedgerAccountArchiveBound, UmiAccountingLedgerAccountArchiveWrite, UmiAccountingLedgerAccountArchiveRead, UmiAccountingLedgerAccountArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_accounting_ledger_account_archive_decode_with_workspace, UmiAccountingLedgerAccount,
    UmiAccountingLedgerAccountArchiveSchema, UmiAccountingLedgerAccountArchiveBound, UmiAccountingLedgerAccountArchiveRead, UmiAccountingLedgerAccountArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_accounting_ledger_account_archive_decode_if_unchanged, umi_accounting_ledger_account_archive_encode,
    UmiAccountingLedgerAccount, UmiAccountingLedgerAccountArchiveSchema, UmiAccountingLedgerAccountArchiveBound, UmiAccountingLedgerAccountArchiveRead, UmiAccountingLedgerAccountArchiveValidate)
