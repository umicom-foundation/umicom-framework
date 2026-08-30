/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_event_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository event invalid semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "umicom/repository/event.h"

int main(void)
{
    UmiRepositoryControlEvent *e = calloc(1U, sizeof(*e));
    assert(e != NULL);
    assert(umi_repository_event_init(e,0U,UMI_REPOSITORY_CONTROL_PLANNED,UMI_STATUS_OK,
     "","x")==UMI_STATUS_INVALID_ARGUMENT);
    free(e);
    return 0;
}
