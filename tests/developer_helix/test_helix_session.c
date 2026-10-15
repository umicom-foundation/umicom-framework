/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_helix_session.c
 *
 * PURPOSE:
 *   Regression coverage for aggregate goal, plan, budgets, checkpoints, validation and lifecycle state for one helix run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/helix_session.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted helix session value. */
static int UmiHelixSessionConstructionCases(void)
{
    UmiHelixSession *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_helix_session_init(value, "session", 3) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_session_init(value, oversized, 3) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_session_init(value, value->id, 3) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "session") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiHelixSessionConstructionCases() != 0) return 1;
 UmiHelixSession s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_init(&s,"session",3)!=UMI_STATUS_OK){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_PLANNED)!=UMI_STATUS_OK||umi_helix_session_transition(&s,UMI_HELIX_READY)!=UMI_STATUS_OK){ return 2; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_RUNNING)!=UMI_STATUS_INVALID_STATE){ return 3; } s.approved=true; s.checkpoint_ready=true; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_session_transition(&s,UMI_HELIX_RUNNING)!=UMI_STATUS_OK||s.iteration!=1U){ return 4; } return 0; }
