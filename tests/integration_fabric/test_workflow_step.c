/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_step.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow step Integration Fabric capability.
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
#include "umicom/integration/fabric/workflow_step.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/workflow_step.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricWorkflowStepTransferEqual(const UmiFabricWorkflowStep *a, const UmiFabricWorkflowStep *b)
{
    return strcmp(a->step_id, b->step_id) == 0 &&
        strcmp(a->operation_id, b->operation_id) == 0 &&
        a->timeout_ms == b->timeout_ms &&
        a->optional == b->optional &&
        a->compensatable == b->compensatable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricWorkflowStepTransferTails(UmiFabricWorkflowStep *value)
{
    (void)value;
    {
        size_t used = strlen(value->step_id) + 1U;
        memset(value->step_id + used, 0xa5, sizeof(value->step_id) - used);
    }
    {
        size_t used = strlen(value->operation_id) + 1U;
        memset(value->operation_id + used, 0xa5, sizeof(value->operation_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricWorkflowStepTransferMalformed(const UmiFabricWorkflowStep *sample)
{
    (void)sample;
    {
        UmiFabricWorkflowStep invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.step_id, 'x', sizeof(invalid.step_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_workflow_step_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_workflow_step_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated step_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricWorkflowStep invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_id, 'x', sizeof(invalid.operation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_workflow_step_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_workflow_step_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricWorkflowStepTransferCases, UmiFabricWorkflowStep,
    umi_fabric_workflow_step_archive_encode, umi_fabric_workflow_step_archive_decode,
    UmiFabricWorkflowStepTransferEqual, UmiFabricWorkflowStepTransferTails, UmiFabricWorkflowStepTransferMalformed)

int main(void) {
    UmiFabricWorkflowStep item;
    CHECK(umi_fabric_workflow_step_init(&item,"reserve","inventory.reserve",1000U,false,true)==UMI_STATUS_OK);
    if (UmiFabricWorkflowStepTransferCases(&item) != 0) return 1;

    CHECK(item.compensatable);
    return 0;
}
