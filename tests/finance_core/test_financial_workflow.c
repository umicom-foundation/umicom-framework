/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_workflow.c
 *
 * PURPOSE:
 *   Exercise the financial workflow financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/finance/core/financial_workflow.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/financial_workflow.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFinancialWorkflowTransferEqual(const UmiFinancialWorkflow *a, const UmiFinancialWorkflow *b)
{
    return strcmp(a->workflow_id.value, b->workflow_id.value) == 0 &&
        strcmp(a->parent_id.value, b->parent_id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->state == b->state &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFinancialWorkflowTransferTails(UmiFinancialWorkflow *value)
{
    (void)value;
    {
        size_t used = strlen(value->workflow_id.value) + 1U;
        memset(value->workflow_id.value + used, 0xa5, sizeof(value->workflow_id.value) - used);
    }
    {
        size_t used = strlen(value->parent_id.value) + 1U;
        memset(value->parent_id.value + used, 0xa5, sizeof(value->parent_id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFinancialWorkflowTransferMalformed(const UmiFinancialWorkflow *sample)
{
    (void)sample;
    {
        UmiFinancialWorkflow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workflow_id.value, 'x', sizeof(invalid.workflow_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_financial_workflow_is_valid(&invalid)) ||
            umi_financial_workflow_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workflow_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFinancialWorkflow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id.value, 'x', sizeof(invalid.parent_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_financial_workflow_is_valid(&invalid)) ||
            umi_financial_workflow_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFinancialWorkflow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_financial_workflow_is_valid(&invalid)) ||
            umi_financial_workflow_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFinancialWorkflowTransferCases, UmiFinancialWorkflow,
    umi_financial_workflow_archive_encode, umi_financial_workflow_archive_decode,
    UmiFinancialWorkflowTransferEqual, UmiFinancialWorkflowTransferTails, UmiFinancialWorkflowTransferMalformed)

int main(void)
{
    UmiFinancialWorkflow x; CHECK(umi_financial_workflow_init(&x,"ID","Name","PARENT",1U)==UMI_STATUS_OK); CHECK(umi_financial_workflow_is_valid(&x));
    if (UmiFinancialWorkflowTransferCases(&x) != 0) return 1;

    return 0;
}
