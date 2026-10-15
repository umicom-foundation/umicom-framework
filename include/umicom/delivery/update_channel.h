/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/update_channel.h
 *
 * PURPOSE:
 *   Control update feed endpoints, release channels and staged rollout cohorts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_UPDATE_CHANNEL_H
#define UMICOM_DELIVERY_UPDATE_CHANNEL_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the update channel data shared with callers of this public contract.
 */
typedef struct UmiUpdateChannel {
    char channel_id[UMI_DELIVERY_ID_CAPACITY];
    char feed_url[UMI_DELIVERY_PATH_CAPACITY];
    UmiReleaseChannel channel;
    unsigned rollout_percentage;
    int allow_prerelease;
    int require_signature;
} UmiUpdateChannel;

/**
 * Initialise update channel from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_update_channel_init(UmiUpdateChannel *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage);
/**
 * Check that update channel satisfies its contract before another service relies on it.
 */
UmiStatus umi_update_channel_validate(const UmiUpdateChannel *channel);
/**
 * Provide the update channel offers operation used by this module and its client
 * applications.
 */
int umi_update_channel_offers(const UmiUpdateChannel *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_update_channel_archive_encode(const UmiUpdateChannel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_update_channel_archive_decode(const void *bytes, size_t byte_count,
    UmiUpdateChannel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_update_channel_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUpdateChannel *expected, UmiUpdateChannel *value);


#ifdef __cplusplus
}
#endif
#endif
