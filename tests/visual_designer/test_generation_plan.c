/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_generation_plan.c
 *
 * PURPOSE:
 *   Validate describe generated declarative/source artifacts before filesystem writes.
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
#include "umicom/designer/visual_designer/generation_plan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/generation_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadGenerationPlanTransferEqual(const UmiRadGenerationPlan *a, const UmiRadGenerationPlan *b)
{
    return strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->output_root, b->output_root) == 0 &&
        a->file_count == b->file_count &&
        a->declarative_enabled == b->declarative_enabled &&
        a->source_enabled == b->source_enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadGenerationPlanTransferTails(UmiRadGenerationPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->output_root) + 1U;
        memset(value->output_root + used, 0xa5, sizeof(value->output_root) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadGenerationPlanTransferMalformed(const UmiRadGenerationPlan *sample)
{
    (void)sample;
    {
        UmiRadGenerationPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_generation_plan_is_valid(&invalid)) ||
            umi_rad_generation_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadGenerationPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.output_root, 'x', sizeof(invalid.output_root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_generation_plan_is_valid(&invalid)) ||
            umi_rad_generation_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated output_root was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadGenerationPlanTransferCases, UmiRadGenerationPlan,
    umi_rad_generation_plan_archive_encode, umi_rad_generation_plan_archive_decode,
    UmiRadGenerationPlanTransferEqual, UmiRadGenerationPlanTransferTails, UmiRadGenerationPlanTransferMalformed)

int main(void){UmiRadGenerationPlan item;CHECK(umi_rad_generation_plan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_generation_plan_is_valid(&item));
    if (UmiRadGenerationPlanTransferCases(&item) != 0) return 1;
return 0;}
