/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ibkr_connection/access_plan.c
 *
 * PURPOSE:
 *   Show local broker-mode defaults without opening a socket or reading credentials.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_plan.h"
#include <stdio.h>

int main(void)
{
    const UmiBrokerAccessMode modes[] = {
        UMI_BROKER_ACCESS_GUEST, UMI_BROKER_ACCESS_PAPER, UMI_BROKER_ACCESS_LIVE
    };
    for (size_t i = 0; i < sizeof modes / sizeof modes[0]; ++i) {
        UmiBrokerAccessPlan plan;
        if (UmiBrokerAccessPlanCreate(modes[i], &plan) != UMI_STATUS_OK) return 1;
        printf("%s: port %u, client %d\n",
            UmiBrokerAccessModeName(plan.mode), (unsigned)plan.port, plan.clientId);
        puts(UmiBrokerAccessAuthenticationGuide(plan.mode));
        UmiIbkrConnectionOptions options;
        UmiStatus status = UmiBrokerAccessPlanConnectionOptions(&plan, false, &options);
        /* Unavailable for Guest and missing acknowledgement for Live are
         * expected refusals. None of these calls open a network connection. */
        if (plan.mode == UMI_BROKER_ACCESS_PAPER && status != UMI_STATUS_OK) return 1;
        if (plan.mode == UMI_BROKER_ACCESS_GUEST && status != UMI_STATUS_UNAVAILABLE) return 1;
        if (plan.mode == UMI_BROKER_ACCESS_LIVE && status != UMI_STATUS_PERMISSION_DENIED) return 1;
    }
    puts("Plans inspected locally. No broker connection was opened.");
    return 0;
}
