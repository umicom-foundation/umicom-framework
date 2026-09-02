/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/tools/umicom/src/wizard_stub.c
 *
 * PURPOSE:
 *   Implement the wizard stub behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "wizard.h"
#include <stdio.h>
/*
 * Perform wizard through the module contract so client applications do not duplicate its
 * policy.
 */
int umi_wizard_run(int argc, char **argv, const char *template_root)
{
    (void)argc; (void)argv; (void)template_root;
    fputs("The GTK4 wizard was not built. Configure again with GTK4 available and UMICOM_BUILD_GTK4=ON.\n", stderr);
    return 1;
}
