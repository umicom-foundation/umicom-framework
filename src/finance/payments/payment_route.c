/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_route.c
 *
 * PURPOSE:
 *   Implement represent selected payment rail, routing priority and route availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_route.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment route from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_route_init(UmiPaymentsPaymentRoute *value,
    const char *id,
    UmiPaymentsRailKind rail_kind,
    uint32_t priority,
    bool available) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->rail_kind=rail_kind;
    value->priority=priority;
    value->available=available;
    return umi_payments_payment_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment route satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_route_valid(const UmiPaymentsPaymentRoute *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;

    return value!=NULL && (value->rail_kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->rail_kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

/*
 * Provide the payments payment route usable operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_route_usable(const UmiPaymentsPaymentRoute *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->available;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentRouteArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbd8ac457716f20d3);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentRoute *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentRouteArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentRoute *)0)->id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiPaymentsPaymentRouteArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentRoute *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->rail_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->available);
}
static void UmiPaymentsPaymentRouteArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentRoute *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->rail_kind = (UmiPaymentsRailKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->available = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPaymentsPaymentRouteArchiveValidate(const UmiPaymentsPaymentRoute *value)
{
    return umi_payments_payment_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_route_archive_encode, umi_payments_payment_route_archive_decode,
    UmiPaymentsPaymentRoute, UmiPaymentsPaymentRouteArchiveSchema, UmiPaymentsPaymentRouteArchiveBound, UmiPaymentsPaymentRouteArchiveWrite, UmiPaymentsPaymentRouteArchiveRead, UmiPaymentsPaymentRouteArchiveValidate)
