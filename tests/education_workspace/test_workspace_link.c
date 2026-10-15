/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_workspace_link.c
 * PURPOSE: Check the transitive link dependencies of the learning panel.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/education_workspace.h"

#include <stdlib.h>

int main(void)
{
    /* Taking both addresses pulls the panel's object into a static-library link.
     * The consumer links only the workspace target: its dependency declarations
     * must supply the inspection UI and its parsers. Volatile pointer objects
     * retain these references when optimisation is enabled. No GTK window is
     * created, so this link regression can run without a graphical session. */
    UmiEducationGtkPanel *(*volatile create_panel)(void) = UmiEducationGtkCreate;
    void (*volatile destroy_panel)(UmiEducationGtkPanel *) = UmiEducationGtkDestroy;

    return create_panel != NULL && destroy_panel != NULL
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
