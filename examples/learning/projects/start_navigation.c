/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/projects/start_navigation.c
 *
 * PURPOSE:
 *   Demonstrate shared Welcome navigation while leaving document ownership unchanged.
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

/* An application keeps its documents in its existing document owner. This
 * example uses an identity string to show that navigation changes only the
 * selected view. It neither opens nor writes a source file. */
static int Step(UmiStartCentreNavigation *state, const char *document,
    UmiStartCentreNavigationAction action, const char *label, int expected)
{
    int show = -1;
    UmiStatus status = UmiStartCentreNavigationUpdate(state, document, action, &show);
    if (status != UMI_STATUS_OK || show != expected) {
        fprintf(stderr, "%s: unexpected status %d or view %d\n", label, (int)status, show);
        return 1;
    }
    printf("%s: %s\n", label, show ? "Welcome" : "Editor");
    return 0;
}
int main(void)
{
    UmiStartCentreNavigation state = {0};
    if (Step(&state, "", UMI_START_CENTRE_OBSERVE, "Empty workspace", 1) ||
        Step(&state, "draft.c", UMI_START_CENTRE_OBSERVE, "Open draft", 0) ||
        Step(&state, "draft.c", UMI_START_CENTRE_SHOW_WELCOME, "Choose Help Welcome", 1) ||
        Step(&state, "draft.c", UMI_START_CENTRE_OBSERVE, "Refresh same document", 1) ||
        Step(&state, "draft.c", UMI_START_CENTRE_SHOW_EDITOR, "Return to editor", 0) ||
        Step(&state, "draft.c", UMI_START_CENTRE_SHOW_WELCOME, "Reopen Welcome", 1) ||
        Step(&state, "other.c", UMI_START_CENTRE_OBSERVE, "Activate another document", 0)) return 1;
    return 0;
}
