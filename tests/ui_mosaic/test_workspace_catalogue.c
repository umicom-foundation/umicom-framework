/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_workspace_catalogue.c
 *
 * PURPOSE:
 *   Exercise workspace catalogue behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/mosaic/workspace_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/workspace_catalogue.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicWorkspaceCatalogueTransferEqual(const UmiUiMosaicWorkspaceCatalogue *a, const UmiUiMosaicWorkspaceCatalogue *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->layout_id, b->layout_id) == 0 &&
        a->application == b->application &&
        a->available == b->available;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicWorkspaceCatalogueTransferTails(UmiUiMosaicWorkspaceCatalogue *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->layout_id) + 1U;
        memset(value->layout_id + used, 0xa5, sizeof(value->layout_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicWorkspaceCatalogueTransferMalformed(const UmiUiMosaicWorkspaceCatalogue *sample)
{
    (void)sample;
    {
        UmiUiMosaicWorkspaceCatalogue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_workspace_catalogue_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_workspace_catalogue_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicWorkspaceCatalogue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_workspace_catalogue_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_workspace_catalogue_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicWorkspaceCatalogue invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.layout_id, 'x', sizeof(invalid.layout_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_workspace_catalogue_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_workspace_catalogue_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated layout_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicWorkspaceCatalogueTransferCases, UmiUiMosaicWorkspaceCatalogue,
    umi_ui_mosaic_workspace_catalogue_archive_encode, umi_ui_mosaic_workspace_catalogue_archive_decode,
    UmiUiMosaicWorkspaceCatalogueTransferEqual, UmiUiMosaicWorkspaceCatalogueTransferTails, UmiUiMosaicWorkspaceCatalogueTransferMalformed)

int main(void) {
    UmiUiMosaicWorkspaceCatalogue value;
    umi_ui_mosaic_workspace_catalogue_init(&value);
    CHECK(umi_ui_mosaic_workspace_catalogue_set(&value, "perspective.workspace_catalogue", "Workspace Catalogue", "layout.default", UMI_UI_MOSAIC_APP_TMS) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_workspace_catalogue_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicWorkspaceCatalogueTransferCases(&value) != 0) return 1;

    return 0;
}
