/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/promotion.c
 *
 * PURPOSE:
 *   Implement promotion of a verified release from one channel to another.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Promotion reuses the same immutable artifact set while changing its approved audience and release policy.
 */

#include "umicom/delivery/promotion.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise promotion from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_promotion_init(UmiPromotion *promotion,
                             const char *release_id,
                             UmiReleaseChannel from_channel,
                             UmiReleaseChannel to_channel)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (promotion == NULL || release_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(promotion, 0, sizeof(*promotion));
    promotion->from_channel = from_channel;
    promotion->to_channel = to_channel;
    promotion->status = UMI_EVIDENCE_UNKNOWN;
    return umi_delivery_copy_text(promotion->release_id, sizeof(promotion->release_id), release_id);
}
/*
 * Check that promotion direction satisfies its contract before another service relies on
 * it.
 */
int umi_promotion_direction_valid(const UmiPromotion *promotion)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (promotion == NULL) return 0;
    if (memchr(promotion->release_id, '\0', sizeof(promotion->release_id)) == NULL) return 0;

    return promotion != NULL && promotion->to_channel >= promotion->from_channel;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPromotionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1222342a26b643a7);
    schema = (schema ^ (uint64_t)sizeof(((UmiPromotion *)0)->release_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPromotionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPromotion *)0)->release_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiPromotionArchiveWrite(UmiArchiveWriter *writer, const UmiPromotion *value)
{
    UmiArchiveWriteText(writer, value->release_id, sizeof(value->release_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->from_channel);
    UmiArchiveWriteSigned(writer, (int64_t)value->to_channel);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
}
static void UmiPromotionArchiveRead(UmiArchiveReader *reader, UmiPromotion *value)
{
    UmiArchiveReadText(reader, value->release_id, sizeof(value->release_id));
    value->from_channel = (UmiReleaseChannel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->to_channel = (UmiReleaseChannel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->status = (UmiEvidenceStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiPromotionArchiveValidate(const UmiPromotion *value)
{
    return umi_promotion_direction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_promotion_direction_archive_encode, umi_promotion_direction_archive_decode,
    UmiPromotion, UmiPromotionArchiveSchema, UmiPromotionArchiveBound, UmiPromotionArchiveWrite, UmiPromotionArchiveRead, UmiPromotionArchiveValidate)
