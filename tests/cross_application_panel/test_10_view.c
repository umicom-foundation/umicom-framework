/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cross_application_panel/test_10_view.c
 *
 * PURPOSE:
 *   Validate cross-application panel view contracts.
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
#include <string.h>
#include "umicom/cross_application_panel/view.h"
#include "umicom/cross_application_panel/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/cross_application_panel/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelIdentityTransferEqual(const UmiPanelIdentity *a, const UmiPanelIdentity *b)
{
    return strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->component_id, b->component_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelIdentityTransferTails(UmiPanelIdentity *value)
{
    (void)value;
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelIdentityTransferMalformed(const UmiPanelIdentity *sample)
{
    (void)sample;
    {
        UmiPanelIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_panel_identity_valid(&invalid)) ||
            umi_panel_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_panel_identity_valid(&invalid)) ||
            umi_panel_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_panel_identity_valid(&invalid)) ||
            umi_panel_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelIdentity invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_panel_identity_valid(&invalid)) ||
            umi_panel_identity_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelIdentityTransferCases, UmiPanelIdentity,
    umi_panel_identity_archive_encode, umi_panel_identity_archive_decode,
    UmiPanelIdentityTransferEqual, UmiPanelIdentityTransferTails, UmiPanelIdentityTransferMalformed)

int main(void)
{
    UmiPanelIdentity identity={0};
    (void)umi_context_copy_text(identity.panel_id,sizeof(identity.panel_id),"panel.test");
    (void)umi_context_copy_text(identity.application_id,sizeof(identity.application_id),"application.test");
    (void)umi_context_copy_text(identity.instance_id,sizeof(identity.instance_id),"instance.test");
    (void)umi_context_copy_text(identity.component_id,sizeof(identity.component_id),"component.test");
    assert(umi_panel_identity_valid(&identity));
    if (UmiPanelIdentityTransferCases(&identity) != 0) return 1;

    assert(strcmp(umi_panel_lifecycle_state_text(UMI_PANEL_VISIBLE),"visible")==0);
    assert(strcmp(umi_panel_placement_text(UMI_PANEL_PLACE_FLOATING),"floating")==0);
    return 0;
}
