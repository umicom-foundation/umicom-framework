/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_compensation_record.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the compensation record Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/compensation_record.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted compensation record value. */
static int UmiFabricCompensationRecordConstructionCases(void)
{
    UmiFabricCompensationRecord *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_compensation_record_init(value, "wf", "s") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->workflow_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_compensation_record_init(value, oversized, "s") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_compensation_record_init(value, value->workflow_id, "s") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->workflow_id, "wf") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->step_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_compensation_record_init(value, "wf", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_compensation_record_init(value, "wf", value->step_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->step_id, "s") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricCompensationRecordConstructionCases() != 0) return 1;

    UmiFabricCompensationRecord r; CHECK(umi_fabric_compensation_record_init(&r,"wf","s")==UMI_STATUS_OK); umi_fabric_compensation_record_attempt(&r,false,1U); umi_fabric_compensation_record_attempt(&r,true,2U); CHECK(r.attempts==2U&&r.succeeded);
    return 0;
}
