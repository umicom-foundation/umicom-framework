/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_style.c
 *
 * PURPOSE:
 *   Exercise the cell style enterprise UI capability.
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
#include "umicom/ui/enterprise/cell_style.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/cell_style.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntCellStyleTransferEqual(const UmiUiEntCellStyle *a, const UmiUiEntCellStyle *b)
{
    return strcmp(a->semantic_role, b->semantic_role) == 0 &&
        a->bold == b->bold &&
        a->italic == b->italic &&
        a->alignment == b->alignment &&
        a->indent == b->indent;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntCellStyleTransferTails(UmiUiEntCellStyle *value)
{
    (void)value;
    {
        size_t used = strlen(value->semantic_role) + 1U;
        memset(value->semantic_role + used, 0xa5, sizeof(value->semantic_role) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntCellStyleTransferMalformed(const UmiUiEntCellStyle *sample)
{
    (void)sample;
    {
        UmiUiEntCellStyle invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.semantic_role, 'x', sizeof(invalid.semantic_role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_cell_style_validate(&invalid)) ||
            umi_ui_ent_cell_style_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated semantic_role was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntCellStyleTransferCases, UmiUiEntCellStyle,
    umi_ui_ent_cell_style_archive_encode, umi_ui_ent_cell_style_archive_decode,
    UmiUiEntCellStyleTransferEqual, UmiUiEntCellStyleTransferTails, UmiUiEntCellStyleTransferMalformed)

int main(void){UmiUiEntCellStyle v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_cell_style_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_cell_style_validate(&v))return 9;
    if (UmiUiEntCellStyleTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
