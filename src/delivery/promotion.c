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
#include "delivery_internal.h"
#include <string.h>
UmiStatus umi_promotion_init(UmiPromotion *promotion,
                             const char *release_id,
                             UmiReleaseChannel from_channel,
                             UmiReleaseChannel to_channel)
{
    if (promotion == NULL || release_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(promotion, 0, sizeof(*promotion));
    promotion->from_channel = from_channel;
    promotion->to_channel = to_channel;
    promotion->status = UMI_EVIDENCE_UNKNOWN;
    return umi_delivery_copy_text(promotion->release_id, sizeof(promotion->release_id), release_id);
}
int umi_promotion_direction_valid(const UmiPromotion *promotion)
{
    return promotion != NULL && promotion->to_channel >= promotion->from_channel;
}
