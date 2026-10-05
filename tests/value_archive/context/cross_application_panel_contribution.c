/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_contribution.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/contribution.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/contribution.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelContributionTransferEqual(const UmiPanelContribution *a, const UmiPanelContribution *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->contribution_id, b->contribution_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->menu_path, b->menu_path) == 0 &&
        strcmp(a->command_id, b->command_id) == 0 &&
        strcmp(a->icon_resource_id, b->icon_resource_id) == 0 &&
        a->enabled == b->enabled &&
        a->priority == b->priority &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelContributionTransferTails(UmiPanelContribution *value)
{
    (void)value;
    {
        size_t used = strlen(value->contribution_id) + 1U;
        memset(value->contribution_id + used, 0xa5, sizeof(value->contribution_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->menu_path) + 1U;
        memset(value->menu_path + used, 0xa5, sizeof(value->menu_path) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->icon_resource_id) + 1U;
        memset(value->icon_resource_id + used, 0xa5, sizeof(value->icon_resource_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelContributionTransferMalformed(const UmiPanelContribution *sample)
{
    (void)sample;
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.contribution_id, 'x', sizeof(invalid.contribution_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated contribution_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.menu_path, 'x', sizeof(invalid.menu_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated menu_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.command_id, 'x', sizeof(invalid.command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated command_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContribution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.icon_resource_id, 'x', sizeof(invalid.icon_resource_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_contribution_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_contribution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated icon_resource_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelContributionTransferCases, UmiPanelContribution,
    umi_panel_contribution_archive_encode, umi_panel_contribution_archive_decode,
    UmiPanelContributionTransferEqual, UmiPanelContributionTransferTails, UmiPanelContributionTransferMalformed)

int main(void)
{
    UmiPanelContribution value;
    umi_panel_contribution_init(&value);
    value.contribution_id[0] = 's';
    value.application_id[0] = 's';
    value.panel_id[0] = 's';
    value.menu_path[0] = 's';
    value.command_id[0] = 's';
    value.icon_resource_id[0] = 's';
    value.enabled = true;
    value.priority = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_contribution_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelContributionTransferCases(&value) != 0) return 1;

    return 0;
}
