/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_update_upgrade.c
 *
 * PURPOSE:
 *   Verify staged update channels and authorised rollback-safe upgrades.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/delivery/delivery.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/update_channel.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUpdateChannelTransferEqual(const UmiUpdateChannel *a, const UmiUpdateChannel *b)
{
    return strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->feed_url, b->feed_url) == 0 &&
        a->channel == b->channel &&
        a->rollout_percentage == b->rollout_percentage &&
        a->allow_prerelease == b->allow_prerelease &&
        a->require_signature == b->require_signature;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUpdateChannelTransferTails(UmiUpdateChannel *value)
{
    (void)value;
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->feed_url) + 1U;
        memset(value->feed_url + used, 0xa5, sizeof(value->feed_url) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUpdateChannelTransferMalformed(const UmiUpdateChannel *sample)
{
    (void)sample;
    {
        UmiUpdateChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_update_channel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_update_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUpdateChannel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.feed_url, 'x', sizeof(invalid.feed_url));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_update_channel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_update_channel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated feed_url was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUpdateChannelTransferCases, UmiUpdateChannel,
    umi_update_channel_archive_encode, umi_update_channel_archive_decode,
    UmiUpdateChannelTransferEqual, UmiUpdateChannelTransferTails, UmiUpdateChannelTransferMalformed)

#include "umicom/delivery/upgrade_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUpgradePlanTransferEqual(const UmiUpgradePlan *a, const UmiUpgradePlan *b)
{
    return strcmp(a->current_version, b->current_version) == 0 &&
        strcmp(a->target_version, b->target_version) == 0 &&
        a->current_generation == b->current_generation &&
        a->target_generation == b->target_generation &&
        a->compatible == b->compatible &&
        a->backup_required == b->backup_required &&
        a->rollback_supported == b->rollback_supported &&
        a->authorised == b->authorised;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUpgradePlanTransferTails(UmiUpgradePlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->current_version) + 1U;
        memset(value->current_version + used, 0xa5, sizeof(value->current_version) - used);
    }
    {
        size_t used = strlen(value->target_version) + 1U;
        memset(value->target_version + used, 0xa5, sizeof(value->target_version) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUpgradePlanTransferMalformed(const UmiUpgradePlan *sample)
{
    (void)sample;
    {
        UmiUpgradePlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.current_version, 'x', sizeof(invalid.current_version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_upgrade_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_upgrade_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated current_version was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUpgradePlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_version, 'x', sizeof(invalid.target_version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_upgrade_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_upgrade_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_version was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUpgradePlanTransferCases, UmiUpgradePlan,
    umi_upgrade_plan_archive_encode, umi_upgrade_plan_archive_decode,
    UmiUpgradePlanTransferEqual, UmiUpgradePlanTransferTails, UmiUpgradePlanTransferMalformed)

int main(void)
{
    UmiUpdateChannel channel;
    UmiUpgradePlan upgrade;
    assert(umi_update_channel_init(
               &channel, "stable", "https://updates.umicom.org/studio/stable.json",
               UMI_RELEASE_STABLE, 25U) == UMI_STATUS_OK);
    assert(umi_update_channel_validate(&channel) == UMI_STATUS_OK);
    if (UmiUpdateChannelTransferCases(&channel) != 0) return 1;

    assert(umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 24U));
    assert(!umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 25U));
    assert(umi_upgrade_plan_init(
               &upgrade, "0.22.0", "0.23.0", 58U, 59U, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_authorise(&upgrade, 0) == UMI_STATUS_UNAVAILABLE);
    assert(umi_upgrade_plan_authorise(&upgrade, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_validate(&upgrade) == UMI_STATUS_OK);
    if (UmiUpgradePlanTransferCases(&upgrade) != 0) return 1;

    assert(umi_upgrade_plan_rollback_generation(&upgrade) == 58U);
    return 0;
}
