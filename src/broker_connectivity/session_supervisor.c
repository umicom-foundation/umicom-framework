/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/session_supervisor.c
 *
 * PURPOSE:
 *   Implement broker session health and reconnect state transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/session_supervisor.h"

void umi_broker_session_supervisor_init(UmiBrokerSessionSupervisor *supervisor)
{
    if (supervisor == NULL) return;
    *supervisor = (UmiBrokerSessionSupervisor){0};
    supervisor->revision = 1U;
}

static void bump(UmiBrokerSessionSupervisor *supervisor)
{
    supervisor->revision =
        supervisor->revision == UINT64_MAX ? 1U : supervisor->revision + 1U;
}

UmiStatus umi_broker_session_supervisor_connected(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    supervisor->state = UMI_BROKER_SESSION_CONNECTED;
    supervisor->lastHeartbeatMilliseconds = nowMilliseconds;
    supervisor->reconnectAttempts = 0U;
    bump(supervisor);
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_session_supervisor_authenticated(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds)
{
    if (supervisor == NULL ||
        (supervisor->state != UMI_BROKER_SESSION_CONNECTED &&
         supervisor->state != UMI_BROKER_SESSION_AUTHENTICATED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    supervisor->state = UMI_BROKER_SESSION_AUTHENTICATED;
    supervisor->lastHeartbeatMilliseconds = nowMilliseconds;
    bump(supervisor);
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_session_supervisor_heartbeat(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds)
{
    if (supervisor == NULL ||
        supervisor->state == UMI_BROKER_SESSION_DISCONNECTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    supervisor->lastHeartbeatMilliseconds = nowMilliseconds;
    if (supervisor->state == UMI_BROKER_SESSION_DEGRADED) {
        supervisor->state = UMI_BROKER_SESSION_CONNECTED;
    }
    bump(supervisor);
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_session_supervisor_disconnected(
    UmiBrokerSessionSupervisor *supervisor)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    supervisor->state = UMI_BROKER_SESSION_DISCONNECTED;
    supervisor->reconnectAttempts += 1U;
    bump(supervisor);
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_session_supervisor_check(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds,
    uint64_t timeoutMilliseconds)
{
    if (supervisor == NULL || timeoutMilliseconds == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (supervisor->state == UMI_BROKER_SESSION_DISCONNECTED) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (nowMilliseconds >= supervisor->lastHeartbeatMilliseconds &&
        nowMilliseconds - supervisor->lastHeartbeatMilliseconds >=
            timeoutMilliseconds) {
        supervisor->state = UMI_BROKER_SESSION_DEGRADED;
        bump(supervisor);
        return UMI_STATUS_TIMEOUT;
    }
    return UMI_STATUS_OK;
}
