/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_designer_workspace.c
 *
 * PURPOSE:
 *   Validate represent the complete visual designer workspace selection and dirty state.
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
#include "umicom/designer/visual_designer/designer_workspace.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/designer_workspace.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDesignerWorkspaceTransferEqual(const UmiRadDesignerWorkspace *a, const UmiRadDesignerWorkspace *b)
{
    return strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->document_id, b->document_id) == 0 &&
        strcmp(a->active_surface_id, b->active_surface_id) == 0 &&
        a->selection_count == b->selection_count &&
        a->dirty == b->dirty;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDesignerWorkspaceTransferTails(UmiRadDesignerWorkspace *value)
{
    (void)value;
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->document_id) + 1U;
        memset(value->document_id + used, 0xa5, sizeof(value->document_id) - used);
    }
    {
        size_t used = strlen(value->active_surface_id) + 1U;
        memset(value->active_surface_id + used, 0xa5, sizeof(value->active_surface_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDesignerWorkspaceTransferMalformed(const UmiRadDesignerWorkspace *sample)
{
    (void)sample;
    {
        UmiRadDesignerWorkspace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_designer_workspace_is_valid(&invalid)) ||
            umi_rad_designer_workspace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadDesignerWorkspace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.document_id, 'x', sizeof(invalid.document_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_designer_workspace_is_valid(&invalid)) ||
            umi_rad_designer_workspace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated document_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadDesignerWorkspace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_surface_id, 'x', sizeof(invalid.active_surface_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_designer_workspace_is_valid(&invalid)) ||
            umi_rad_designer_workspace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_surface_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDesignerWorkspaceTransferCases, UmiRadDesignerWorkspace,
    umi_rad_designer_workspace_archive_encode, umi_rad_designer_workspace_archive_decode,
    UmiRadDesignerWorkspaceTransferEqual, UmiRadDesignerWorkspaceTransferTails, UmiRadDesignerWorkspaceTransferMalformed)

int main(void){UmiRadDesignerWorkspace item;CHECK(umi_rad_designer_workspace_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_designer_workspace_is_valid(&item));
    if (UmiRadDesignerWorkspaceTransferCases(&item) != 0) return 1;
return 0;}
