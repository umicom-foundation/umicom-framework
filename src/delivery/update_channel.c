/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update_channel.c
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
#include "umicom/delivery/update_channel.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise update channel from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_update_channel_init(UmiUpdateChannel *channel,
                                      const char *channel_id,
                                      const char *feed_url,
                                      UmiReleaseChannel release_channel,
                                      unsigned rollout_percentage)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL || channel_id == NULL || feed_url == NULL ||
        rollout_percentage > 100U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(channel, 0, sizeof(*channel));
    status = umi_delivery_copy_text(channel->channel_id,
                                    sizeof(channel->channel_id), channel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(channel->feed_url,
                                    sizeof(channel->feed_url), feed_url);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    channel->channel = release_channel;
    channel->rollout_percentage = rollout_percentage;
    channel->allow_prerelease = release_channel != UMI_RELEASE_STABLE;
    channel->require_signature = release_channel == UMI_RELEASE_STABLE;
    return UMI_STATUS_OK;
}

/* Check that update channel satisfies its contract before another service relies on it. */
UmiStatus umi_update_channel_validate(const UmiUpdateChannel *channel)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(channel->channel_id, '\0', sizeof(channel->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(channel->feed_url, '\0', sizeof(channel->feed_url)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (channel->channel_id[0] == '\0' || channel->feed_url[0] == '\0' ||
        channel->channel < UMI_RELEASE_DEVELOPMENT ||
        channel->channel > UMI_RELEASE_STABLE ||
        channel->rollout_percentage > 100U) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (channel->channel == UMI_RELEASE_STABLE &&
        !channel->require_signature) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the update channel offers operation used by this module and its client
 * applications.
 */
int umi_update_channel_offers(const UmiUpdateChannel *channel,
                                 UmiReleaseChannel release_channel,
                                 unsigned cohort)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_update_channel_validate(channel) != UMI_STATUS_OK ||
        cohort >= 100U || release_channel != channel->channel ||
        cohort >= channel->rollout_percentage) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (release_channel != UMI_RELEASE_STABLE && !channel->allow_prerelease) {
        return 0;
    }
    return 1;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiUpdateChannelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa01208f97f5399eb);
    schema = (schema ^ (uint64_t)sizeof(((UmiUpdateChannel *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiUpdateChannel *)0)->feed_url)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiUpdateChannelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiUpdateChannel *)0)->channel_id) - 1U +
        8U + sizeof(((UmiUpdateChannel *)0)->feed_url) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiUpdateChannelArchiveWrite(UmiArchiveWriter *writer, const UmiUpdateChannel *value)
{
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->feed_url, sizeof(value->feed_url));
    UmiArchiveWriteSigned(writer, (int64_t)value->channel);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rollout_percentage);
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_prerelease);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_signature);
}
static void UmiUpdateChannelArchiveRead(UmiArchiveReader *reader, UmiUpdateChannel *value)
{
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->feed_url, sizeof(value->feed_url));
    value->channel = (UmiReleaseChannel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rollout_percentage = (unsigned)UmiArchiveReadUnsigned(reader, UINT_MAX);
    value->allow_prerelease = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_signature = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiUpdateChannelArchiveValidate(const UmiUpdateChannel *value)
{
    return umi_update_channel_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_update_channel_archive_encode, umi_update_channel_archive_decode,
    UmiUpdateChannel, UmiUpdateChannelArchiveSchema, UmiUpdateChannelArchiveBound, UmiUpdateChannelArchiveWrite, UmiUpdateChannelArchiveRead, UmiUpdateChannelArchiveValidate)
