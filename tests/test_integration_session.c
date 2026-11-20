/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_session.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/session.h"
#include <string.h>

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

/* Failed admission preserves the previous session. An intentional
 * replacement can borrow the existing session strings safely. */
static int TestSessionAdmission(void)
{
    UmiIntegrationSession session;
    CHECK(umi_integration_session_init(&session, "studio", "local-user") == UMI_STATUS_OK);
    UmiIntegrationSession before;
    memcpy(&before, &session, sizeof before);
    char longId[UMI_INTEGRATION_ID_CAPACITY + 1U];
    memset(longId, 'a', sizeof longId - 1U);
    longId[sizeof longId - 1U] = '\0';
    CHECK(umi_integration_session_init(&session, "new", longId) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&session, &before, sizeof session) == 0);
    CHECK(umi_integration_session_init(&session, longId, "new") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&session, &before, sizeof session) == 0);
    CHECK(umi_integration_session_init(&session, "", "new") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&session, &before, sizeof session) == 0);
    CHECK(umi_integration_session_init(&session, session.identity_id, session.session_id) == UMI_STATUS_OK);
    CHECK(strcmp(session.session_id, "local-user") == 0);
    CHECK(strcmp(session.identity_id, "studio") == 0 && session.next_correlation == 1U);
    return 0;
}

/* Check every short capacity at each final decimal value and retry with the
 * exact required bytes. Exhaustion must preserve the destination and state. */
static int TestCorrelationBoundaries(void)
{
    const char *const expected[] = {"session-18446744073709551614", "session-18446744073709551615"};
    UmiIntegrationSession session;
    CHECK(umi_integration_session_init(&session, "session", "owner") == UMI_STATUS_OK);
    session.next_correlation = UINT64_MAX - 1U;
    char output[64], marker[sizeof output];
    memset(marker, 'X', sizeof marker);
    for (size_t index = 0; index < sizeof expected / sizeof expected[0]; ++index) {
        size_t needed = strlen(expected[index]) + 1U;
        uint64_t original = session.next_correlation;
        memcpy(output, marker, sizeof output);
        CHECK(umi_integration_session_next_correlation(&session, output, 0) == UMI_STATUS_INVALID_ARGUMENT);
        for (size_t capacity = 1U; capacity < needed; ++capacity) {
            CHECK(umi_integration_session_next_correlation(&session, output, capacity) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(session.next_correlation == original && memcmp(output, marker, sizeof output) == 0);
        }
        CHECK(umi_integration_session_next_correlation(&session, output, needed) == UMI_STATUS_OK);
        CHECK(strcmp(output, expected[index]) == 0 && output[needed] == 'X');
    }
    CHECK(session.next_correlation == 0U);
    CHECK(umi_integration_session_next_correlation(&session, output, sizeof output) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_integration_session_next_correlation(&session, output, sizeof output) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(output, expected[1]) == 0 && session.next_correlation == 0U);
    return 0;
}

/* Distinguish malformed stored state from a valid long name that requires
 * more bytes than the envelope correlation field provides. */
static int TestBoundedSessionStorage(void)
{
    UmiIntegrationSession session = {0};
    char output[UMI_INTEGRATION_ID_CAPACITY + 21U] = "keep";
    CHECK(umi_integration_session_next_correlation(&session, output, sizeof output) == UMI_STATUS_INVALID_STATE);
    memset(session.session_id, 's', sizeof session.session_id);
    session.next_correlation = UINT64_MAX;
    CHECK(umi_integration_session_next_correlation(&session, output, sizeof output) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(output, "keep") == 0 && session.next_correlation == UINT64_MAX);
    session.session_id[sizeof session.session_id - 1U] = '\0';
    CHECK(umi_integration_session_init(&session, session.session_id, "owner") == UMI_STATUS_OK);
    session.next_correlation = UINT64_MAX;
    CHECK(umi_integration_session_next_correlation(&session, output, UMI_INTEGRATION_ID_CAPACITY) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(output, "keep") == 0 && session.next_correlation == UINT64_MAX);
    CHECK(umi_integration_session_next_correlation(&session, output, sizeof output) == UMI_STATUS_OK);
    CHECK(strlen(output) == sizeof output - 1U);
    CHECK(strncmp(output, session.session_id, UMI_INTEGRATION_ID_CAPACITY - 1U) == 0);
    CHECK(strcmp(output + UMI_INTEGRATION_ID_CAPACITY - 1U, "-18446744073709551615") == 0);
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationSession s; char id[128];
    CHECK(umi_integration_session_init(&s,"session","user")==UMI_STATUS_OK);
    CHECK(umi_integration_session_next_correlation(&s,id,sizeof(id))==UMI_STATUS_OK);
    CHECK(strcmp(id,"session-1")==0);
    /* Keep the ordinary ID check and exercise the state transitions around it. */
    CHECK(TestSessionAdmission() == 0);
    CHECK(TestCorrelationBoundaries() == 0);
    CHECK(TestBoundedSessionStorage() == 0);
    puts("Checked session admission, complete correlation IDs and terminal counter exhaustion.");
    return 0;
}
