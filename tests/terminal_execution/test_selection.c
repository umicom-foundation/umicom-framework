/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/test_selection.c
 * PURPOSE: Reject stale terminal decisions without launching a command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal_ui/selection.h"
#include "../terminal_fixture.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%d: %s\n", __LINE__, #value); failed = 1; goto cleanup; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    int failed = 0;
    UmiTerminalTestFixture fixture = {0};
    UmiTerminalSelection captured, current, untouched;
    CHECK(terminal_fixture_create(&fixture) == UMI_STATUS_OK);
    CHECK(UmiTerminalSelectionCapture(fixture.controller, &captured) == UMI_STATUS_OK);
    UmiTerminalSession *session = umi_terminal_controller_active_session(fixture.controller);
    if (strcmp(argv[1], "stable") == 0) {
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, &current) == UMI_STATUS_OK);
        CHECK(current.controller_revision == captured.controller_revision);
    } else if (strcmp(argv[1], "switch-return") == 0) {
        CHECK(umi_terminal_controller_open(fixture.controller, "", "fixture.second", "Second", ".") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_terminal_controller_activate(fixture.controller, "fixture.primary") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "recycle") == 0) {
        /* The controller deliberately keeps one terminal tab alive. A spare
         * session permits closing and recreating the captured identity. */
        CHECK(umi_terminal_controller_open(fixture.controller, "", "fixture.spare", "Spare", ".") == UMI_STATUS_OK);
        CHECK(umi_terminal_controller_activate(fixture.controller, "fixture.primary") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, &captured) == UMI_STATUS_OK);
        CHECK(umi_terminal_controller_close(fixture.controller, "fixture.primary") == UMI_STATUS_OK);
        CHECK(umi_terminal_controller_open(fixture.controller, "", "fixture.primary", "Replacement", ".") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "directory") == 0) {
        CHECK(umi_terminal_session_set_working_directory(session, "..") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "closed") == 0) {
        CHECK(umi_terminal_session_close(session) == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
        memset(&current, 0xa5, sizeof current); memcpy(&untouched, &current, sizeof untouched);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, &current) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&current, &untouched, sizeof current) == 0);
    } else if (strcmp(argv[1], "empty") == 0) {
/* Respect the last-tab rule and exercise missing backing-session validation separately. The previous statement is retained for review. */
#if 0
        CHECK(umi_terminal_controller_close(fixture.controller, "fixture.primary") == UMI_STATUS_OK);
#endif
        CHECK(umi_terminal_controller_close(fixture.controller, "fixture.primary") == UMI_STATUS_BUSY);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_OK);
        /* Removing the session through its shared manager simulates an absent
         * backing session without changing the last-tab UI contract. */
        CHECK(umi_terminal_manager_close(fixture.manager, "fixture.primary") == UMI_STATUS_OK);
        memset(&current, 0xa5, sizeof current); memcpy(&untouched, &current, sizeof untouched);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, &current) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&current, &untouched, sizeof current) == 0);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "transcript") == 0) {
        CHECK(umi_terminal_transcript_append(umi_terminal_session_transcript(session), 0U,
            UMI_TERMINAL_STREAM_OUTPUT, "new output") == UMI_STATUS_OK);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "malformed") == 0) {
        memset(captured.session.session_id, 'x', sizeof captured.session.session_id);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, &captured) == UMI_STATUS_OK);
        memset(captured.session.working_directory, 'x', sizeof captured.session.working_directory);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, &captured) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "arguments") == 0) {
        CHECK(UmiTerminalSelectionCapture(NULL, &captured) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTerminalSelectionCapture(fixture.controller, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTerminalSelectionValidate(NULL, &captured) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTerminalSelectionValidate(fixture.controller, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else CHECK(0);
cleanup:
    terminal_fixture_destroy(&fixture);
    return failed;
}
