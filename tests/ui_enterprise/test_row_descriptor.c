/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_descriptor.c
 *
 * PURPOSE:
 *   Exercise the row descriptor enterprise UI capability.
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
#include "umicom/ui/enterprise/row_descriptor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/row_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntRowDescriptorTransferEqual(const UmiUiEntRowDescriptor *a, const UmiUiEntRowDescriptor *b)
{
    return a->row_key == b->row_key &&
        strcmp(a->label, b->label) == 0 &&
        a->selectable == b->selectable &&
        a->editable == b->editable &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntRowDescriptorTransferTails(UmiUiEntRowDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntRowDescriptorTransferMalformed(const UmiUiEntRowDescriptor *sample)
{
    (void)sample;
    {
        UmiUiEntRowDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_row_descriptor_validate(&invalid)) ||
            umi_ui_ent_row_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntRowDescriptorTransferCases, UmiUiEntRowDescriptor,
    umi_ui_ent_row_descriptor_archive_encode, umi_ui_ent_row_descriptor_archive_decode,
    UmiUiEntRowDescriptorTransferEqual, UmiUiEntRowDescriptorTransferTails, UmiUiEntRowDescriptorTransferMalformed)

int main(void){UmiUiEntRowDescriptor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_row_descriptor_init(&v)!=UMI_STATUS_OK)return 1;v.row_key=42U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_row_descriptor_validate(&v))return 9;
    if (UmiUiEntRowDescriptorTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
