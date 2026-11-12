/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_session_admission.c
 *
 * PURPOSE:
 *   Check monotonic session clocks, bounded counters and stable repeated timeout observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/session_supervisor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *operation = argv[1], *mode = argv[2];
    UmiBrokerSessionSupervisor s, before;
    umi_broker_session_supervisor_init(&s);
    CHECK(umi_broker_session_supervisor_connected(&s, 100U) == UMI_STATUS_OK);
    CHECK(umi_broker_session_supervisor_authenticated(&s, 100U) == UMI_STATUS_OK);
    uint64_t now = 200U;
    UmiStatus expected = strcmp(operation, "check") == 0 ? UMI_STATUS_TIMEOUT : UMI_STATUS_OK;
    int unchanged = 0;
    if (strcmp(mode, "revision") == 0) { s.revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; unchanged = 1; }
    else if (strcmp(mode, "invalid-state") == 0) { s.state = (UmiBrokerSessionState)99; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "zero-revision") == 0) { s.revision = 0U; expected = UMI_STATUS_INVALID_STATE; unchanged = 1; }
    else if (strcmp(mode, "clock") == 0) { now = 99U; expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "attempts") == 0) { s.reconnectAttempts = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; unchanged = 1; }
    else if (strcmp(mode, "repeated-timeout") == 0) { s.state = UMI_BROKER_SESSION_DEGRADED; s.revision = UINT64_MAX; unchanged = 1; }
    else if (strcmp(mode, "healthy-at-limit") == 0) { s.revision = UINT64_MAX; now = 110U; expected = UMI_STATUS_OK; unchanged = 1; }
    else if (strcmp(mode, "zero-timeout") == 0) { expected = UMI_STATUS_INVALID_ARGUMENT; unchanged = 1; }
    else if (strcmp(mode, "disconnected") == 0) { s.state = UMI_BROKER_SESSION_DISCONNECTED; expected = UMI_STATUS_UNAVAILABLE; unchanged = 1; }
    else if (strcmp(mode, "recover") == 0) s.state = UMI_BROKER_SESSION_DEGRADED;
    else if (strcmp(mode, "normal") != 0) return 2;
    memcpy(&before, &s, sizeof(s));
    UmiStatus status;
    if (strcmp(operation, "connect") == 0) status = umi_broker_session_supervisor_connected(&s, now);
    else if (strcmp(operation, "authenticate") == 0) status = umi_broker_session_supervisor_authenticated(&s, now);
    else if (strcmp(operation, "heartbeat") == 0) status = umi_broker_session_supervisor_heartbeat(&s, now);
    else if (strcmp(operation, "disconnect") == 0) status = umi_broker_session_supervisor_disconnected(&s);
    else if (strcmp(operation, "check") == 0) status = umi_broker_session_supervisor_check(&s, now, strcmp(mode, "zero-timeout") == 0 ? 0U : 50U);
    else return 2;
    CHECK(status == expected);
    if (unchanged) CHECK(memcmp(&before, &s, sizeof(s)) == 0);
    else {
        CHECK(s.revision == before.revision + 1U);
        CHECK(UmiBrokerSessionSupervisorValidate(&s) == UMI_STATUS_OK);
        if (strcmp(mode, "recover") == 0) CHECK(s.state == UMI_BROKER_SESSION_CONNECTED);
        if (strcmp(operation, "disconnect") == 0) CHECK(s.reconnectAttempts == 1U);
        if (strcmp(operation, "check") == 0) CHECK(s.state == UMI_BROKER_SESSION_DEGRADED);
    }
    return 0;
}
