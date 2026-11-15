/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_start_navigation.c
 *
 * PURPOSE:
 *   Verify explicit Welcome navigation, changed document identity and transactional refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_workbench/start_centre.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    const char *modes[] = {"empty", "document", "welcome", "repeat", "observe", "switch",
        "close", "return", "empty-return", "arguments", "state", "bounds", "alias"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    if (!known) return 2;
    const char *mode = argv[1];
    UmiStartCentreNavigation state = {0};
    int show = 91;
    if (strcmp(mode, "empty") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(&state, "", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_OK);
        CHECK(show == 1 && state.welcome_requested == 0); return 0;
    }
    CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_OK);
    CHECK(show == 0 && strcmp(state.active_document, "draft.c") == 0);
    if (strcmp(mode, "document") == 0) return 0;
    CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_SHOW_WELCOME, &show) == UMI_STATUS_OK);
    CHECK(show == 1 && state.welcome_requested == 1);
    UmiStartCentreNavigation before = state;
    if (strcmp(mode, "welcome") == 0) return 0;
    if (strcmp(mode, "repeat") == 0 || strcmp(mode, "observe") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", strcmp(mode, "repeat") == 0
            ? UMI_START_CENTRE_SHOW_WELCOME : UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_OK);
        CHECK(show == 1 && memcmp(&state, &before, sizeof state) == 0); return 0;
    }
    if (strcmp(mode, "switch") == 0 || strcmp(mode, "close") == 0) {
        int closing = strcmp(mode, "close") == 0;
        CHECK(UmiStartCentreNavigationUpdate(&state, closing ? "" : "other.c", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_OK);
        CHECK(show == closing && state.welcome_requested == 0); return 0;
    }
    if (strcmp(mode, "return") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_SHOW_EDITOR, &show) == UMI_STATUS_OK);
        CHECK(show == 0 && state.welcome_requested == 0); return 0;
    }
    if (strcmp(mode, "empty-return") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(&state, "", UMI_START_CENTRE_SHOW_EDITOR, &show) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "arguments") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(NULL, "draft.c", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiStartCentreNavigationUpdate(&state, NULL, UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_OBSERVE, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", (UmiStartCentreNavigationAction)99, &show) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "state") == 0) {
        state.welcome_requested = 2; before = state;
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&state, &before, sizeof state) == 0);
        state.welcome_requested = 1;
        memset(state.active_document, 'x', sizeof state.active_document); before = state;
        CHECK(UmiStartCentreNavigationUpdate(&state, "draft.c", UMI_START_CENTRE_OBSERVE, &show) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "bounds") == 0) {
        char id[UMI_START_CENTRE_DOCUMENT_CAPACITY]; memset(id, 'a', sizeof id);
        CHECK(UmiStartCentreNavigationUpdate(&state, id, UMI_START_CENTRE_SHOW_WELCOME, &show) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(show == 1 && memcmp(&state, &before, sizeof state) == 0);
        id[sizeof id - 1U] = '\0';
        CHECK(UmiStartCentreNavigationUpdate(&state, id, UMI_START_CENTRE_SHOW_WELCOME, &show) == UMI_STATUS_OK);
        CHECK(strlen(state.active_document) == sizeof id - 1U && show == 1); return 0;
    } else if (strcmp(mode, "alias") == 0) {
        CHECK(UmiStartCentreNavigationUpdate(&state, state.active_document, UMI_START_CENTRE_SHOW_EDITOR, &show) == UMI_STATUS_OK);
        CHECK(strcmp(state.active_document, "draft.c") == 0 && show == 0); return 0;
    }
    CHECK(show == 1 && memcmp(&state, &before, sizeof state) == 0);
    return 0;
}
