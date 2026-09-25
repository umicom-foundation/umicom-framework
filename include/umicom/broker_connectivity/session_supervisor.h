/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/session_supervisor.h
 *
 * PURPOSE:
 *   Track broker connection/authentication heartbeat and reconnect state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_SESSION_SUPERVISOR_H
#define UMICOM_BROKER_CONNECTIVITY_SESSION_SUPERVISOR_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiBrokerSessionState {
    UMI_BROKER_SESSION_DISCONNECTED = 0,
    UMI_BROKER_SESSION_CONNECTING = 1,
    UMI_BROKER_SESSION_CONNECTED = 2,
    UMI_BROKER_SESSION_AUTHENTICATED = 3,
    UMI_BROKER_SESSION_DEGRADED = 4
} UmiBrokerSessionState;

typedef struct UmiBrokerSessionSupervisor {
    UmiBrokerSessionState state;
    uint64_t lastHeartbeatMilliseconds;
    uint64_t reconnectAttempts;
    uint64_t revision;
} UmiBrokerSessionSupervisor;

void umi_broker_session_supervisor_init(UmiBrokerSessionSupervisor *supervisor);
UmiStatus umi_broker_session_supervisor_connected(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds);
UmiStatus umi_broker_session_supervisor_authenticated(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds);
UmiStatus umi_broker_session_supervisor_heartbeat(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds);
UmiStatus umi_broker_session_supervisor_disconnected(
    UmiBrokerSessionSupervisor *supervisor);
UmiStatus umi_broker_session_supervisor_check(
    UmiBrokerSessionSupervisor *supervisor,
    uint64_t nowMilliseconds,
    uint64_t timeoutMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
