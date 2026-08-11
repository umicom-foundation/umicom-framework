/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/promotion.h
 *
 * PURPOSE:
 *   Represent promotion of a verified release from one channel to another.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Promotion reuses the same immutable artifact set while changing its approved audience and release policy.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PROMOTION_H
#define INCLUDE_UMICOM_DELIVERY_PROMOTION_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPromotion {
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel from_channel;
    UmiReleaseChannel to_channel;
    UmiEvidenceStatus status;
} UmiPromotion;
UmiStatus umi_promotion_init(UmiPromotion *promotion,
                             const char *release_id,
                             UmiReleaseChannel from_channel,
                             UmiReleaseChannel to_channel);
int umi_promotion_direction_valid(const UmiPromotion *promotion);

#ifdef __cplusplus
}
#endif

#endif
