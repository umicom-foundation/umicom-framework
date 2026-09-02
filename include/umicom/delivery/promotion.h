/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/promotion.h
 *
 * PURPOSE:
 *   Represent promotion of a verified release from one channel to another.
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

#ifndef INCLUDE_UMICOM_DELIVERY_PROMOTION_H
#define INCLUDE_UMICOM_DELIVERY_PROMOTION_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the promotion data shared with callers of this public contract.
 */
typedef struct UmiPromotion {
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel from_channel;
    UmiReleaseChannel to_channel;
    UmiEvidenceStatus status;
} UmiPromotion;
/**
 * Initialise promotion from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_promotion_init(UmiPromotion *promotion,
                             const char *release_id,
                             UmiReleaseChannel from_channel,
                             UmiReleaseChannel to_channel);
/**
 * Check that promotion direction satisfies its contract before another service relies on
 * it.
 */
int umi_promotion_direction_valid(const UmiPromotion *promotion);

#ifdef __cplusplus
}
#endif

#endif
