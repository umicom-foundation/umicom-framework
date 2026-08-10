/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_catalogue.c
 *
 * PURPOSE:
 *   Display the canonical Framework capability catalogue and create a sample
 *   suite manifest demonstrating independent applications under one suite.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"

#include <stdio.h>
#include <string.h>

int umi_cli_command_capabilities(UmiCliContext *context, int argc, char **argv)
{
    size_t index;
    (void)context;
    (void)argc;
    (void)argv;
    (void)printf("Umicom Framework %s capability catalogue:\n",
                 UMICOM_FRAMEWORK_VERSION_STRING);
    for (index = 0U;
         index < umi_framework_capability_catalogue_count();
         ++index) {
        const UmiFrameworkCapabilityDefinition *definition =
            umi_framework_capability_catalogue_at(index);
        if (definition != NULL) {
            (void)printf("  %-38s %-12s %s\n",
                         definition->capability_id,
                         umi_capability_maturity_text(definition->maturity),
                         definition->category);
        }
    }
    return 0;
}

int umi_cli_command_suite(UmiCliContext *context, int argc, char **argv)
{
    UmiSuite suite;
    UmiSuiteApplication application;
    UmiStatus status;
    const char *path;
    (void)context;

    if (argc < 2 || strcmp(argv[0], "sample") != 0) {
        (void)fprintf(stderr, "Usage: umicom suite sample PATH\n");
        return 2;
    }
    path = argv[1];
    umi_suite_init(&suite, "org.umicom.suite", "Umicom Application Suite");

    (void)memset(&application, 0, sizeof(application));
    (void)snprintf(application.id, sizeof(application.id),
                   "%s", "org.umicom.studio");
    (void)snprintf(application.name, sizeof(application.name),
                   "%s", "Umicom Studio IDE");
    (void)snprintf(application.path, sizeof(application.path),
                   "%s", "applications/studio");
    (void)snprintf(application.executable, sizeof(application.executable),
                   "%s", "umicom-studio-ide");
    application.enabled = 1;
    (void)umi_suite_add(&suite, &application);

    (void)memset(&application, 0, sizeof(application));
    (void)snprintf(application.id, sizeof(application.id),
                   "%s", "org.umicom.designer");
    (void)snprintf(application.name, sizeof(application.name),
                   "%s", "Umicom Designer");
    (void)snprintf(application.path, sizeof(application.path),
                   "%s", "applications/designer");
    (void)snprintf(application.executable, sizeof(application.executable),
                   "%s", "umicom-designer");
    application.enabled = 1;
    (void)umi_suite_add(&suite, &application);

    status = umi_suite_write_manifest(&suite, path);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Suite manifest failed: %s\n",
                      umi_status_text(status));
        return 1;
    }
    (void)printf("Created suite manifest: %s\n", path);
    return 0;
}
