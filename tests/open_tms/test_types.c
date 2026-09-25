/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_types.c
 *
 * PURPOSE:
 *   Verify Open TMS input defaults and snapshot validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/open_tms/types.h"
int main(void)
{
    UmiOpenTmsInput input; UmiOpenTmsSnapshot snapshot;
    umi_open_tms_input_init(&input); assert(input.trusted==1);
    umi_open_tms_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id,"cash.forecast-net"); snapshot.score=75.0; snapshot.ready=1;
    assert(umi_open_tms_snapshot_validate(&snapshot)==UMI_STATUS_OK);
    snapshot.score=101.0;
    assert(umi_open_tms_snapshot_validate(&snapshot)==UMI_STATUS_INVALID_STATE);
    return 0;
}
