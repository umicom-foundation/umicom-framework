/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_permission.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/permission.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/permission.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelPermissionTransferEqual(const UmiPanelPermission *a, const UmiPanelPermission *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->permission_id, b->permission_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        a->allow_open == b->allow_open &&
        a->allow_close == b->allow_close &&
        a->allow_move == b->allow_move &&
        a->allow_rebind == b->allow_rebind &&
        a->allow_cross_application == b->allow_cross_application &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelPermissionTransferTails(UmiPanelPermission *value)
{
    (void)value;
    {
        size_t used = strlen(value->permission_id) + 1U;
        memset(value->permission_id + used, 0xa5, sizeof(value->permission_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelPermissionTransferMalformed(const UmiPanelPermission *sample)
{
    (void)sample;
    {
        UmiPanelPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.permission_id, 'x', sizeof(invalid.permission_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated permission_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelPermission invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_permission_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_permission_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelPermissionTransferCases, UmiPanelPermission,
    umi_panel_permission_archive_encode, umi_panel_permission_archive_decode,
    UmiPanelPermissionTransferEqual, UmiPanelPermissionTransferTails, UmiPanelPermissionTransferMalformed)

int main(void)
{
    UmiPanelPermission value;
    umi_panel_permission_init(&value);
    value.permission_id[0] = 's';
    value.panel_id[0] = 's';
    value.subject_id[0] = 's';
    value.allow_open = true;
    value.allow_close = true;
    value.allow_move = true;
    value.allow_rebind = true;
    value.allow_cross_application = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_permission_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelPermissionTransferCases(&value) != 0) return 1;

    return 0;
}
