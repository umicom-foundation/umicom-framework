/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/application_module/main.c
 * PURPOSE: Demonstrate a thin application consuming Framework module metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/runtime/application_module.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    UmiApplicationModuleDescriptor module;
    /* The application accepts one chosen source file. Framework owns parsing,
     * file bounds and platform behaviour; no Git command is executed here. */
    if (argc != 2) {
        (void)fprintf(stderr, "Usage: umicom-module-example ABSOLUTE_MANIFEST_PATH\n");
        return 2;
    }
    UmiStatus status = umi_application_module_load(argv[1], &module);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot inspect manifest: %s\n", umi_status_text(status));
        return 1;
    }
    (void)printf("%s\n%s\nInstaller component: %s\n",
        module.name, module.option, module.component);
    return 0;
}
