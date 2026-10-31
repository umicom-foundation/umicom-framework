/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_connection_guidance.c
 *
 * PURPOSE:
 *   Verify connection advice distinguishes readiness, provider refusal, stale and incomplete observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/connection_guidance.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static UmiIbkrConnectionSnapshot snapshot, before;

static int Expect(UmiIbkrConnectionStep expected)
{
    UmiIbkrConnectionGuidance out = {0};
    before = snapshot;
    CHECK(UmiIbkrConnectionExplain(&snapshot, &out) == UMI_STATUS_OK);
    CHECK(out.step == expected && out.title != NULL && out.detail != NULL);
    CHECK(out.title[0] != '\0' && out.detail[0] != '\0');
    CHECK(memcmp(&snapshot, &before, sizeof(snapshot)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    if (strcmp(name, "invalid") == 0) {
        UmiIbkrConnectionGuidance out = {UMI_IBKR_STEP_WAIT, "keep title", "keep detail"};
        UmiIbkrConnectionGuidance original = out;
        CHECK(UmiIbkrConnectionExplain(NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.step == original.step && out.title == original.title && out.detail == original.detail);
        CHECK(UmiIbkrConnectionExplain(&snapshot, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        snapshot.state = (UmiIbkrConnectionState)999;
        CHECK(UmiIbkrConnectionExplain(&snapshot, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.step == original.step && out.title == original.title && out.detail == original.detail);
    } else if (strcmp(name, "idle") == 0) {
        snapshot.stale = true;
        CHECK(Expect(UMI_IBKR_STEP_OPEN) == 0);
    } else if (strcmp(name, "waiting") == 0) {
        const UmiIbkrConnectionState states[] = {UMI_IBKR_CONNECTING, UMI_IBKR_HANDSHAKE, UMI_IBKR_WAITING};
        for (size_t i = 0U; i < sizeof(states) / sizeof(states[0]); ++i) {
            snapshot.state = states[i]; snapshot.stale = true;
            CHECK(Expect(UMI_IBKR_STEP_WAIT) == 0);
        }
    } else if (strcmp(name, "endpoint") == 0) {
        snapshot.state = UMI_IBKR_FAILED; snapshot.lastStatus = UMI_STATUS_IO_ERROR;
        CHECK(Expect(UMI_IBKR_STEP_CHECK_ENDPOINT) == 0);
        snapshot.providerCode = 502;
        CHECK(Expect(UMI_IBKR_STEP_CHECK_ENDPOINT) == 0);
        snapshot.providerCode = 0; snapshot.protocolVersion = 176;
        CHECK(Expect(UMI_IBKR_STEP_RECONNECT) == 0);
    } else if (strcmp(name, "duplicate-client") == 0) {
        snapshot.state = UMI_IBKR_FAILED; snapshot.providerCode = 326;
        CHECK(Expect(UMI_IBKR_STEP_CHANGE_CLIENT) == 0);
    } else if (strcmp(name, "protocol") == 0) {
        snapshot.state = UMI_IBKR_FAILED; snapshot.lastStatus = UMI_STATUS_PARSE_ERROR;
        CHECK(Expect(UMI_IBKR_STEP_REVIEW_PROTOCOL) == 0);
        snapshot.lastStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(Expect(UMI_IBKR_STEP_REVIEW_PROTOCOL) == 0);
    } else if (strcmp(name, "provider") == 0) {
        snapshot.state = UMI_IBKR_FAILED; snapshot.providerCode = 1100;
        CHECK(Expect(UMI_IBKR_STEP_REVIEW_PROVIDER) == 0);
        snapshot.providerCode = 98765;
        CHECK(Expect(UMI_IBKR_STEP_REVIEW_PROVIDER) == 0);
    } else if (strcmp(name, "stale") == 0) {
        snapshot.state = UMI_IBKR_DISCONNECTED;
        CHECK(Expect(UMI_IBKR_STEP_RECONNECT) == 0);
        snapshot.state = UMI_IBKR_READY; snapshot.stale = true;
        snapshot.requestIssued = true; snapshot.summaryComplete = true; snapshot.positionsComplete = true;
        CHECK(Expect(UMI_IBKR_STEP_RECONNECT) == 0);
    } else if (strcmp(name, "ready") == 0) {
        snapshot.state = UMI_IBKR_READY;
        CHECK(Expect(UMI_IBKR_STEP_READ_ACCOUNT) == 0);
    } else if (strcmp(name, "partial") == 0) {
        snapshot.state = UMI_IBKR_READY; snapshot.requestIssued = true;
        CHECK(Expect(UMI_IBKR_STEP_WAIT_REPORT) == 0);
        snapshot.summaryComplete = true;
        CHECK(Expect(UMI_IBKR_STEP_WAIT_REPORT) == 0);
        snapshot.summaryComplete = false; snapshot.positionsComplete = true;
        CHECK(Expect(UMI_IBKR_STEP_WAIT_REPORT) == 0);
    } else if (strcmp(name, "complete") == 0) {
        snapshot.state = UMI_IBKR_READY; snapshot.requestIssued = true;
        snapshot.summaryComplete = true; snapshot.positionsComplete = true;
        CHECK(Expect(UMI_IBKR_STEP_REVIEW_REPORT) == 0);
    } else if (strcmp(name, "healthy-code") == 0) {
        snapshot.state = UMI_IBKR_READY; snapshot.providerCode = 2104;
        CHECK(Expect(UMI_IBKR_STEP_READ_ACCOUNT) == 0);
        snapshot.providerCode = 326;
        CHECK(Expect(UMI_IBKR_STEP_READ_ACCOUNT) == 0);
    } else return 2;
    return 0;
}
