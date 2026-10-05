/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_sort_descriptor.c
 *
 * PURPOSE:
 *   Exercise the sort descriptor enterprise UI capability.
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
#include "umicom/ui/enterprise/sort_descriptor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/sort_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntSortDescriptorTransferEqual(const UmiUiEntSortDescriptor *a, const UmiUiEntSortDescriptor *b)
{
    return strcmp(a->column_id, b->column_id) == 0 &&
        a->direction == b->direction &&
        a->priority == b->priority &&
        a->case_sensitive == b->case_sensitive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntSortDescriptorTransferTails(UmiUiEntSortDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->column_id) + 1U;
        memset(value->column_id + used, 0xa5, sizeof(value->column_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntSortDescriptorTransferMalformed(const UmiUiEntSortDescriptor *sample)
{
    (void)sample;
    {
        UmiUiEntSortDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.column_id, 'x', sizeof(invalid.column_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_sort_descriptor_validate(&invalid)) ||
            umi_ui_ent_sort_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated column_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntSortDescriptorTransferCases, UmiUiEntSortDescriptor,
    umi_ui_ent_sort_descriptor_archive_encode, umi_ui_ent_sort_descriptor_archive_decode,
    UmiUiEntSortDescriptorTransferEqual, UmiUiEntSortDescriptorTransferTails, UmiUiEntSortDescriptorTransferMalformed)

int main(void){UmiUiEntSortDescriptor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_sort_descriptor_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_sort_descriptor_validate(&v))return 9;
    if (UmiUiEntSortDescriptorTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
