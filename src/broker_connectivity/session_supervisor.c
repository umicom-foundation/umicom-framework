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
#include <stddef.h>

void umi_broker_session_supervisor_init(UmiBrokerSessionSupervisor *supervisor)
{
    if (supervisor == NULL) return;
    *supervisor = (UmiBrokerSessionSupervisor){0};
    supervisor->revision = 1U;
}

/* Preserve the former state machine for review. The replacement retains its
 * connection stages, adds monotonic clock/counter admission and makes repeated
 * timeout observations stable instead of wrapping a revision back to one. */
#if 0
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

#endif

UmiStatus UmiBrokerSessionSupervisorValidate(const UmiBrokerSessionSupervisor *supervisor)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((unsigned)supervisor->state > (unsigned)UMI_BROKER_SESSION_DEGRADED ||
        supervisor->revision == 0U) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
static UmiStatus SessionTime(const UmiBrokerSessionSupervisor *supervisor, uint64_t now)
{
    UmiStatus status = UmiBrokerSessionSupervisorValidate(supervisor);
    if (status != UMI_STATUS_OK) return status;
    return now < supervisor->lastHeartbeatMilliseconds ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
UmiStatus umi_broker_session_supervisor_connected(UmiBrokerSessionSupervisor *supervisor, uint64_t now)
{
    UmiStatus status = SessionTime(supervisor, now);
    if (status != UMI_STATUS_OK) return status;
    if (supervisor->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    supervisor->state = UMI_BROKER_SESSION_CONNECTED;
    supervisor->lastHeartbeatMilliseconds = now;
    supervisor->reconnectAttempts = 0U;
    ++supervisor->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_broker_session_supervisor_authenticated(UmiBrokerSessionSupervisor *supervisor, uint64_t now)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = SessionTime(supervisor, now);
    if (status != UMI_STATUS_OK) return status;
    if (supervisor->state != UMI_BROKER_SESSION_CONNECTED &&
        supervisor->state != UMI_BROKER_SESSION_AUTHENTICATED) return UMI_STATUS_INVALID_STATE;
    if (supervisor->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    supervisor->state = UMI_BROKER_SESSION_AUTHENTICATED;
    supervisor->lastHeartbeatMilliseconds = now;
    ++supervisor->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_broker_session_supervisor_heartbeat(UmiBrokerSessionSupervisor *supervisor, uint64_t now)
{
    if (supervisor == NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = SessionTime(supervisor, now);
    if (status != UMI_STATUS_OK) return status;
    if (supervisor->state == UMI_BROKER_SESSION_DISCONNECTED) return UMI_STATUS_INVALID_STATE;
    if (supervisor->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    supervisor->lastHeartbeatMilliseconds = now;
    if (supervisor->state == UMI_BROKER_SESSION_DEGRADED)
        supervisor->state = UMI_BROKER_SESSION_CONNECTED;
    ++supervisor->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_broker_session_supervisor_disconnected(UmiBrokerSessionSupervisor *supervisor)
{
    UmiStatus status = UmiBrokerSessionSupervisorValidate(supervisor);
    if (status != UMI_STATUS_OK) return status;
    if (supervisor->revision == UINT64_MAX || supervisor->reconnectAttempts == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    supervisor->state = UMI_BROKER_SESSION_DISCONNECTED;
    ++supervisor->reconnectAttempts;
    ++supervisor->revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_broker_session_supervisor_check(UmiBrokerSessionSupervisor *supervisor,
                                             uint64_t now, uint64_t timeout)
{
    if (supervisor == NULL || timeout == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SessionTime(supervisor, now);
    if (status != UMI_STATUS_OK) return status;
    if (supervisor->state == UMI_BROKER_SESSION_DISCONNECTED) return UMI_STATUS_UNAVAILABLE;
    if (now - supervisor->lastHeartbeatMilliseconds >= timeout) {
        /* Repeated observation of the same degraded state adds no information.
         * Keep TIMEOUT while preserving the existing revision, even at capacity. */
        if (supervisor->state == UMI_BROKER_SESSION_DEGRADED) return UMI_STATUS_TIMEOUT;
        if (supervisor->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        supervisor->state = UMI_BROKER_SESSION_DEGRADED;
        ++supervisor->revision;
        return UMI_STATUS_TIMEOUT;
    }
    return UMI_STATUS_OK;
}
