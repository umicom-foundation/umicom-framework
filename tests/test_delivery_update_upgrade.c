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
#include "value_archive/construction_cases.h"
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

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiUpdateChannelTransferCasesWithWorkspace, UmiUpdateChannel,
    umi_update_channel_archive_encode, umi_update_channel_archive_decode_with_workspace, UmiUpdateChannelTransferEqual)

#include "value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiUpdateChannelConditionalArchiveCases, UmiUpdateChannel,
    umi_update_channel_archive_encode, umi_update_channel_archive_decode_if_unchanged,
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

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiUpgradePlanTransferCasesWithWorkspace, UmiUpgradePlan,
    umi_upgrade_plan_archive_encode, umi_upgrade_plan_archive_decode_with_workspace, UmiUpgradePlanTransferEqual)

#include "value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiUpgradePlanConditionalArchiveCases, UmiUpgradePlan,
    umi_upgrade_plan_archive_encode, umi_upgrade_plan_archive_decode_if_unchanged,
    UmiUpgradePlanTransferEqual, UmiUpgradePlanTransferTails, UmiUpgradePlanTransferMalformed)



/* A rejected replacement must preserve the previously accepted upgrade plan value. */
static int UmiUpgradePlanConstructionCases(void)
{
    UmiUpgradePlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_upgrade_plan_init(value, "0.22.0", "0.23.0", 58U, 59U, 1) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->current_version) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_upgrade_plan_init(value, oversized, "0.23.0", 58U, 59U, 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_upgrade_plan_init(value, value->current_version, "0.23.0", 58U, 59U, 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->current_version, "0.22.0") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target_version) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_upgrade_plan_init(value, "0.22.0", oversized, 58U, 59U, 1) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_upgrade_plan_init(value, "0.22.0", value->target_version, 58U, 59U, 1) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_version, "0.23.0") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiUpgradePlanConstructionCases() != 0) return 1;

    UmiUpdateChannel channel;
    UmiUpgradePlan upgrade;
    assert(umi_update_channel_init(
               &channel, "stable", "https://updates.umicom.org/studio/stable.json",
               UMI_RELEASE_STABLE, 25U) == UMI_STATUS_OK);
    assert(umi_update_channel_validate(&channel) == UMI_STATUS_OK);
    if (UmiUpdateChannelTransferCases(&channel) != 0) return 1;
    if (UmiUpdateChannelTransferCasesWithWorkspace(&channel) != 0) return 1;
    if (UmiUpdateChannelConditionalArchiveCases(&channel) != 0) return 1;

    assert(umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 24U));
    assert(!umi_update_channel_offers(&channel, UMI_RELEASE_STABLE, 25U));
    assert(umi_upgrade_plan_init(
               &upgrade, "0.22.0", "0.23.0", 58U, 59U, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_authorise(&upgrade, 0) == UMI_STATUS_UNAVAILABLE);
    assert(umi_upgrade_plan_authorise(&upgrade, 1) == UMI_STATUS_OK);
    assert(umi_upgrade_plan_validate(&upgrade) == UMI_STATUS_OK);
    if (UmiUpgradePlanTransferCases(&upgrade) != 0) return 1;
    if (UmiUpgradePlanTransferCasesWithWorkspace(&upgrade) != 0) return 1;
    if (UmiUpgradePlanConditionalArchiveCases(&upgrade) != 0) return 1;

    assert(umi_upgrade_plan_rollback_generation(&upgrade) == 58U);
    return 0;
}
