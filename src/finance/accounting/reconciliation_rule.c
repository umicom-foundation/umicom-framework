/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/reconciliation_rule.c
 *
 * PURPOSE:
 *   Implement define reconciliation tolerance and automatic matching policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/reconciliation_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise accounting reconciliation rule from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_accounting_reconciliation_rule_init(UmiAccountingReconciliationRule *value,
    const char *id,
    int64_t tolerance_minor,
    bool auto_match) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->tolerance_minor=tolerance_minor;
    value->auto_match=auto_match;
    return umi_accounting_reconciliation_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting reconciliation rule satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_reconciliation_rule_valid(const UmiAccountingReconciliationRule *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;

    return value!=NULL && (value->tolerance_minor>=0);
}

/*
 * Provide the accounting reconciliation rule automatic operation used by this module and
 * its client applications.
 */
bool umi_accounting_reconciliation_rule_automatic(const UmiAccountingReconciliationRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->auto_match;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAccountingReconciliationRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1c623af7bf7fec0e);
    schema = (schema ^ (uint64_t)sizeof(((UmiAccountingReconciliationRule *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAccountingReconciliationRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAccountingReconciliationRule *)0)->id.value) - 1U +
        8U +
        8U;
}
static void UmiAccountingReconciliationRuleArchiveWrite(UmiArchiveWriter *writer, const UmiAccountingReconciliationRule *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->tolerance_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->auto_match);
}
static void UmiAccountingReconciliationRuleArchiveRead(UmiArchiveReader *reader, UmiAccountingReconciliationRule *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->tolerance_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->auto_match = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiAccountingReconciliationRuleArchiveValidate(const UmiAccountingReconciliationRule *value)
{
    return umi_accounting_reconciliation_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_accounting_reconciliation_rule_archive_encode, umi_accounting_reconciliation_rule_archive_decode,
    UmiAccountingReconciliationRule, UmiAccountingReconciliationRuleArchiveSchema, UmiAccountingReconciliationRuleArchiveBound, UmiAccountingReconciliationRuleArchiveWrite, UmiAccountingReconciliationRuleArchiveRead, UmiAccountingReconciliationRuleArchiveValidate)
