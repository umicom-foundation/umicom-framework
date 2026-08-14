/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_fix_venue_contracts.c
 *
 * PURPOSE:
 *   Validate fix venue contracts behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/trading/trading.h"
static UmiStatus route(void *instance,const UmiOrderRequest *request){return instance!=NULL&&request!=NULL?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
int main(void){
    int state=1;UmiVenueAdapter v={&state,"SIM",route};assert(umi_venue_adapter_valid(&v));
    UmiFixSessionInfo f={0};(void)snprintf(f.sender_comp_id,sizeof(f.sender_comp_id),"%s","UMI");(void)snprintf(f.target_comp_id,sizeof(f.target_comp_id),"%s","VENUE");f.next_out_sequence=1;f.next_in_sequence=1;assert(umi_fix_session_info_valid(&f));return 0;
}
