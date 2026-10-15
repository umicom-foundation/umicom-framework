/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_workflow_execution.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the workflow execution Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/workflow_execution.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted workflow execution value. */
static int UmiFabricWorkflowExecutionConstructionCases(void)
{
    UmiFabricWorkflowExecution *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_workflow_execution_init(value, "wf") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->workflow_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_workflow_execution_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_workflow_execution_init(value, value->workflow_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->workflow_id, "wf") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricWorkflowExecutionConstructionCases() != 0) return 1;

    UmiFabricWorkflowExecution e; CHECK(umi_fabric_workflow_execution_init(&e,"wf")==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_RUNNING)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_FAILED)==UMI_STATUS_OK); CHECK(umi_fabric_workflow_execution_transition(&e,UMI_FABRIC_WORKFLOW_COMPENSATING)==UMI_STATUS_OK);
    return 0;
}
