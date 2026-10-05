/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/posting_rule.c
 *
 * PURPOSE:
 *   Implement map canonical accounting event types to debit and credit ledger accounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise accounting posting rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_posting_rule_init(UmiAccountingPostingRule *value,
    const char *id,
    const char *event_type,
    const char *debit_account_id,
    const char *credit_account_id,
    bool active) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->event_type,sizeof value->event_type,event_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_accounting_id_assign(&value->debit_account_id,debit_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_accounting_id_assign(&value->credit_account_id,credit_account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->active=active;
    return umi_accounting_posting_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting posting rule satisfies its contract before another service relies
 * on it.
 */
bool umi_accounting_posting_rule_valid(const UmiAccountingPostingRule *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->event_type, '\0', sizeof(value->event_type)) == NULL) return 0;
    if (memchr(value->debit_account_id.value, '\0', sizeof(value->debit_account_id.value)) == NULL) return 0;
    if (memchr(value->credit_account_id.value, '\0', sizeof(value->credit_account_id.value)) == NULL) return 0;

    return value!=NULL && (value->event_type[0]!='\0' && umi_financial_id_compare(&value->debit_account_id,&value->credit_account_id)!=0);
}

/*
 * Provide the accounting posting rule usable operation used by this module and its client
 * applications.
 */
bool umi_accounting_posting_rule_usable(const UmiAccountingPostingRule *value) {
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
static uint64_t UmiAccountingPostingRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8493bdb6ef44aa8e);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingPostingRule *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingPostingRule *)0)->event_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingPostingRule *)0)->debit_account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingPostingRule *)0)->credit_account_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAccountingPostingRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAccountingPostingRule *)0)->id.value) - 1U +
        8U + sizeof(((UmiAccountingPostingRule *)0)->event_type) - 1U +
        8U + sizeof(((UmiAccountingPostingRule *)0)->debit_account_id.value) - 1U +
        8U + sizeof(((UmiAccountingPostingRule *)0)->credit_account_id.value) - 1U +
        8U;
}
static void UmiAccountingPostingRuleArchiveWrite(UmiArchiveWriter *writer, const UmiAccountingPostingRule *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->event_type, sizeof(value->event_type));
    UmiArchiveWriteText(writer, value->debit_account_id.value, sizeof(value->debit_account_id.value));
    UmiArchiveWriteText(writer, value->credit_account_id.value, sizeof(value->credit_account_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiAccountingPostingRuleArchiveRead(UmiArchiveReader *reader, UmiAccountingPostingRule *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->event_type, sizeof(value->event_type));
    UmiArchiveReadText(reader, value->debit_account_id.value, sizeof(value->debit_account_id.value));
    UmiArchiveReadText(reader, value->credit_account_id.value, sizeof(value->credit_account_id.value));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAccountingPostingRuleArchiveValidate(const UmiAccountingPostingRule *value)
{
    return umi_accounting_posting_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_accounting_posting_rule_archive_encode, umi_accounting_posting_rule_archive_decode,
    UmiAccountingPostingRule, UmiAccountingPostingRuleArchiveSchema, UmiAccountingPostingRuleArchiveBound, UmiAccountingPostingRuleArchiveWrite, UmiAccountingPostingRuleArchiveRead, UmiAccountingPostingRuleArchiveValidate)
