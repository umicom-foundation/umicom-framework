/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_flaky_test_model.c
 *
 * PURPOSE:
 *   Implement the test flaky test model behavior for
 *   Umicom Framework.
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
#include "umicom/test/workbench/flaky_test_model.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/test/workbench/flaky_test_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFlakyTestModelTransferEqual(const UmiFlakyTestModel *a, const UmiFlakyTestModel *b)
{
    return strcmp(a->value.id, b->value.id) == 0 &&
        strcmp(a->value.label, b->value.label) == 0 &&
        strcmp(a->value.detail, b->value.detail) == 0 &&
        a->value.state == b->value.state &&
        a->value.flags == b->value.flags &&
        a->value.score == b->value.score &&
        a->value.duration_us == b->value.duration_us &&
        a->value.revision == b->value.revision &&
        a->generation == b->generation &&
        a->item_count == b->item_count &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFlakyTestModelTransferTails(UmiFlakyTestModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->value.id) + 1U;
        memset(value->value.id + used, 0xa5, sizeof(value->value.id) - used);
    }
    {
        size_t used = strlen(value->value.label) + 1U;
        memset(value->value.label + used, 0xa5, sizeof(value->value.label) - used);
    }
    {
        size_t used = strlen(value->value.detail) + 1U;
        memset(value->value.detail + used, 0xa5, sizeof(value->value.detail) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFlakyTestModelTransferMalformed(const UmiFlakyTestModel *sample)
{
    (void)sample;
    {
        UmiFlakyTestModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.id, 'x', sizeof(invalid.value.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_flaky_test_model_valid(&invalid)) ||
            umi_flaky_test_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFlakyTestModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.label, 'x', sizeof(invalid.value.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_flaky_test_model_valid(&invalid)) ||
            umi_flaky_test_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFlakyTestModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.detail, 'x', sizeof(invalid.value.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_flaky_test_model_valid(&invalid)) ||
            umi_flaky_test_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFlakyTestModelTransferCases, UmiFlakyTestModel,
    umi_flaky_test_model_archive_encode, umi_flaky_test_model_archive_decode,
    UmiFlakyTestModelTransferEqual, UmiFlakyTestModelTransferTails, UmiFlakyTestModelTransferMalformed)

int main(void){UmiFlakyTestModel m;assert(umi_flaky_test_model_init(&m,"flaky_test_model","Flaky Test Model")==UMI_STATUS_OK);assert(umi_flaky_test_model_set_active(&m,true)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_count(&m,3U)==UMI_STATUS_OK);assert(umi_flaky_test_model_set_state(&m,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_flaky_test_model_valid(&m));
    if (UmiFlakyTestModelTransferCases(&m) != 0) return 1;
assert(m.active);assert(m.item_count==3U);return 0;}
