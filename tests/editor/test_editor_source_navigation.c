/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_source_navigation.c
 *
 * PURPOSE:
 *   Implement the test editor source navigation behavior for
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
 * Umicom Framework source location and navigation history tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_history.h"

/*
 * Exercise location and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorSourceLocation location(const char *uri,
                                        uint64_t line,
                                        uint64_t column)
{
    UmiEditorSourceLocation value;
    assert(umi_editor_source_location_initialize(&value,
                                                  uri,
                                                  line,
                                                  column) == UMI_STATUS_OK);
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorNavigationHistory *history = NULL;
    UmiEditorNavigationHistorySnapshot snapshot;
    UmiEditorSourceLocation first = location("file:///project/alpha.c", 4U, 2U);
    UmiEditorSourceLocation second = location("file:///project/beta.c", 8U, 1U);
    UmiEditorSourceLocation third = location("file:///project/gamma.c", 12U, 0U);
    UmiEditorSourceLocation branch = location("file:///project/branch.c", 3U, 7U);
    UmiEditorSourceLocation current;
    char formatted[1200];

    assert(umi_editor_source_location_validate(&first) == UMI_STATUS_OK);
    assert(umi_editor_source_location_format(&first,
                                              formatted,
                                              sizeof(formatted)) ==
           UMI_STATUS_OK);
    assert(strcmp(formatted, "file:///project/alpha.c:5:3") == 0);
    assert(umi_editor_navigation_history_create(3U, &history) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &first) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &second) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_record(history, &third) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_go_back(history, &current) == UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&current, &second));
    assert(umi_editor_navigation_history_record(history, &branch) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_go_forward(history, &current) ==
           UMI_STATUS_NOT_FOUND);
    assert(umi_editor_navigation_history_go_back(history, &current) == UMI_STATUS_OK);
    assert(umi_editor_source_location_same_position(&current, &second));
    assert(umi_editor_navigation_history_snapshot(history, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.count == 3U);
    assert(snapshot.can_go_back);
    assert(snapshot.can_go_forward);

    /* Adjacent visits to one position update metadata without growing history. */
    assert(umi_editor_navigation_history_go_forward(history, &current) == UMI_STATUS_OK);
    (void)strcpy(branch.preview, "branch preview");
    assert(umi_editor_navigation_history_record(history, &branch) == UMI_STATUS_OK);
    assert(umi_editor_navigation_history_snapshot(history, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.count == 3U);
    assert(umi_editor_navigation_history_current(history, &current) == UMI_STATUS_OK);
    assert(strcmp(current.preview, "branch preview") == 0);

    umi_editor_navigation_history_destroy(history);
    return 0;
}
