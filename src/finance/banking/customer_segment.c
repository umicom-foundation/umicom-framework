/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/customer_segment.c
 *
 * PURPOSE:
 *   Implement assign a reusable banking customer segment for product and service policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/customer_segment.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise banking customer segment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_customer_segment_init(UmiBankingCustomerSegment *value,
    const char *id,
    const char *customer_id,
    UmiBankingSegment segment) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->segment=segment;
    return umi_banking_customer_segment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking customer segment satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_customer_segment_valid(const UmiBankingCustomerSegment *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->customer_id.value, '\0', sizeof(value->customer_id.value)) == NULL) return 0;

    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->segment>=UMI_BANKING_SEGMENT_RETAIL && value->segment<=UMI_BANKING_SEGMENT_INSTITUTIONAL);
}

/*
 * Provide the banking customer segment institutional operation used by this module and its
 * client applications.
 */
bool umi_banking_customer_segment_institutional(const UmiBankingCustomerSegment *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->segment==UMI_BANKING_SEGMENT_INSTITUTIONAL;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiBankingCustomerSegmentArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x94a651547cc06701);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingCustomerSegment *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiBankingCustomerSegment *)0)->customer_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiBankingCustomerSegmentArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiBankingCustomerSegment *)0)->id.value) - 1U +
        8U + sizeof(((UmiBankingCustomerSegment *)0)->customer_id.value) - 1U +
        8U;
}
static void UmiBankingCustomerSegmentArchiveWrite(UmiArchiveWriter *writer, const UmiBankingCustomerSegment *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->customer_id.value, sizeof(value->customer_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->segment);
}
static void UmiBankingCustomerSegmentArchiveRead(UmiArchiveReader *reader, UmiBankingCustomerSegment *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->customer_id.value, sizeof(value->customer_id.value));
    value->segment = (UmiBankingSegment)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiBankingCustomerSegmentArchiveValidate(const UmiBankingCustomerSegment *value)
{
    return umi_banking_customer_segment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_banking_customer_segment_archive_encode, umi_banking_customer_segment_archive_decode,
    UmiBankingCustomerSegment, UmiBankingCustomerSegmentArchiveSchema, UmiBankingCustomerSegmentArchiveBound, UmiBankingCustomerSegmentArchiveWrite, UmiBankingCustomerSegmentArchiveRead, UmiBankingCustomerSegmentArchiveValidate)
