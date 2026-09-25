/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_session_supervisor.c
 *
 * PURPOSE:
 *   Verify connect, authenticate, heartbeat, timeout and reconnect state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/broker_connectivity/session_supervisor.h"

int main(void)
{
    UmiBrokerSessionSupervisor supervisor;
    umi_broker_session_supervisor_init(&supervisor);
    assert(umi_broker_session_supervisor_connected(&supervisor, 1000U) == UMI_STATUS_OK);
    assert(umi_broker_session_supervisor_authenticated(&supervisor, 1100U) == UMI_STATUS_OK);
    assert(supervisor.state == UMI_BROKER_SESSION_AUTHENTICATED);
    assert(umi_broker_session_supervisor_check(&supervisor, 2000U, 5000U) == UMI_STATUS_OK);
    assert(umi_broker_session_supervisor_check(&supervisor, 7000U, 5000U) == UMI_STATUS_TIMEOUT);
    assert(supervisor.state == UMI_BROKER_SESSION_DEGRADED);
    assert(umi_broker_session_supervisor_disconnected(&supervisor) == UMI_STATUS_OK);
    assert(supervisor.reconnectAttempts == 1U);
    return 0;
}
