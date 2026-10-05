/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_rail.c
 *
 * PURPOSE:
 *   Implement describe payment-network capabilities and transaction limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment rail from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_payments_payment_rail_init(UmiPaymentsPaymentRail *value,
    const char *id,
    UmiPaymentsRailKind kind,
    const char *name,
    int64_t maximum_minor,
    bool supports_instant) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->kind=kind;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->maximum_minor=maximum_minor;
    value->supports_instant=supports_instant;
    return umi_payments_payment_rail_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment rail satisfies its contract before another service relies on
 * it.
 */
bool umi_payments_payment_rail_valid(const UmiPaymentsPaymentRail *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;

    return value!=NULL && (value->name[0]!='\0' && value->maximum_minor>0 && value->kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

/*
 * Provide the payments payment rail instant operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_rail_instant(const UmiPaymentsPaymentRail *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->supports_instant;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentRailArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4af563343744976c);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentRail *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentRail *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentRailArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentRail *)0)->id.value) - 1U +
        8U +
        8U + sizeof(((UmiPaymentsPaymentRail *)0)->name) - 1U +
        8U +
        8U;
}
static void UmiPaymentsPaymentRailArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentRail *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_instant);
}
static void UmiPaymentsPaymentRailArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentRail *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->kind = (UmiPaymentsRailKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->maximum_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->supports_instant = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPaymentsPaymentRailArchiveValidate(const UmiPaymentsPaymentRail *value)
{
    return umi_payments_payment_rail_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_rail_archive_encode, umi_payments_payment_rail_archive_decode,
    UmiPaymentsPaymentRail, UmiPaymentsPaymentRailArchiveSchema, UmiPaymentsPaymentRailArchiveBound, UmiPaymentsPaymentRailArchiveWrite, UmiPaymentsPaymentRailArchiveRead, UmiPaymentsPaymentRailArchiveValidate)
