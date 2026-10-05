/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_aggregation_descriptor.c
 *
 * PURPOSE:
 *   Exercise the aggregation descriptor enterprise UI capability.
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
#include "umicom/ui/enterprise/aggregation_descriptor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/aggregation_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntAggregationDescriptorTransferEqual(const UmiUiEntAggregationDescriptor *a, const UmiUiEntAggregationDescriptor *b)
{
    return strcmp(a->aggregation_id, b->aggregation_id) == 0 &&
        strcmp(a->column_id, b->column_id) == 0 &&
        a->kind == b->kind;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntAggregationDescriptorTransferTails(UmiUiEntAggregationDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->aggregation_id) + 1U;
        memset(value->aggregation_id + used, 0xa5, sizeof(value->aggregation_id) - used);
    }
    {
        size_t used = strlen(value->column_id) + 1U;
        memset(value->column_id + used, 0xa5, sizeof(value->column_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntAggregationDescriptorTransferMalformed(const UmiUiEntAggregationDescriptor *sample)
{
    (void)sample;
    {
        UmiUiEntAggregationDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.aggregation_id, 'x', sizeof(invalid.aggregation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_aggregation_descriptor_validate(&invalid)) ||
            umi_ui_ent_aggregation_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated aggregation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntAggregationDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.column_id, 'x', sizeof(invalid.column_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_aggregation_descriptor_validate(&invalid)) ||
            umi_ui_ent_aggregation_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated column_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntAggregationDescriptorTransferCases, UmiUiEntAggregationDescriptor,
    umi_ui_ent_aggregation_descriptor_archive_encode, umi_ui_ent_aggregation_descriptor_archive_decode,
    UmiUiEntAggregationDescriptorTransferEqual, UmiUiEntAggregationDescriptorTransferTails, UmiUiEntAggregationDescriptorTransferMalformed)

int main(void){UmiUiEntAggregationDescriptor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_aggregation_descriptor_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.aggregation_id,sizeof v.aggregation_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"value")!=UMI_STATUS_OK)return 3;v.kind=UMI_UI_ENT_AGG_SUM;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_aggregation_descriptor_validate(&v))return 9;
    if (UmiUiEntAggregationDescriptorTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
