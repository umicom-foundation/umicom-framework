/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_controller.c
 *
 * PURPOSE:
 *   Implement the test terminal controller behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework integrated terminal controller tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "terminal_fixture.h"

int main(void)
{
    UmiTerminalTestFixture fixture;
    UmiTerminalControllerSnapshot snapshot;
    UmiTerminalSearchQuery query;
    UmiTerminalSearchResult result;
    UmiTerminalSession *active;

    assert(terminal_fixture_create(&fixture) == UMI_STATUS_OK);
    assert(umi_terminal_controller_snapshot(fixture.controller, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.sessions == 1U && snapshot.tabs == 1U);
    assert(snapshot.profiles >= 1U && snapshot.events == 1U);
    assert(snapshot.operation_graph.node_count == 2U);
    assert(snapshot.operation_graph.ready_count == 1U);
    assert(umi_build_graph_start(fixture.operations, "configure") == UMI_STATUS_OK);
    assert(umi_terminal_controller_cancel_operation(
        fixture.controller, "configure") == UMI_STATUS_OK);
    assert(umi_terminal_controller_retry_operation(
        fixture.controller, "configure") == UMI_STATUS_OK);

    assert(umi_terminal_controller_open(fixture.controller, "",
        "fixture.second", "Second", ".") == UMI_STATUS_OK);
    assert(strcmp(umi_terminal_tab_model_active_id(
        umi_terminal_controller_tabs(fixture.controller)), "fixture.second") == 0);
    assert(umi_terminal_controller_activate(fixture.controller,
        "fixture.primary") == UMI_STATUS_OK);
    assert(umi_terminal_controller_split_active(fixture.controller,
        "fixture.split", "Split", UMI_TERMINAL_ORIENTATION_VERTICAL) ==
        UMI_STATUS_OK);
    assert(umi_terminal_controller_snapshot(fixture.controller, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.sessions == 3U && snapshot.tabs == 3U);
    assert(snapshot.split_nodes == 3U);

    active = umi_terminal_controller_active_session(fixture.controller);
    assert(active != NULL);
    assert(umi_terminal_transcript_append(umi_terminal_session_transcript(active),
        10U, UMI_TERMINAL_STREAM_ERROR,
        "src/main.c:9:2: error: fixture failure") == UMI_STATUS_OK);
    umi_terminal_search_query_init(&query);
    (void)snprintf(query.text, sizeof(query.text), "%s", "fixture failure");
    assert(umi_terminal_controller_search_active(fixture.controller,
        &query, &result) == UMI_STATUS_OK);
    assert(result.count == 1U);

    assert(umi_terminal_controller_close_active(fixture.controller) ==
           UMI_STATUS_OK);
    assert(umi_terminal_controller_snapshot(fixture.controller, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.sessions == 2U && snapshot.split_nodes == 1U);
    assert(umi_terminal_controller_clear_history(fixture.controller) ==
           UMI_STATUS_OK);
    terminal_fixture_destroy(&fixture);
    return 0;
}
