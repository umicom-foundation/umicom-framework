/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_application_manifest.c
 *
 * PURPOSE:
 *   Verify application manifest loading, validation, frontend recognition, and
 *   required capability discovery using a temporary deterministic fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char temporary[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char message[256];
    UmiApplicationManifest manifest;
    const char *document =
        "schema: umicom.application.v1\n"
        "application:\n"
        "  id: org.umicom.designer\n"
        "  name: Umicom Designer\n"
        "  short_name: Designer\n"
        "  version: 0.1.0\n"
        "  executable: umicom-designer\n"
        "framework:\n"
        "  minimum_version: 0.9.0\n"
        "frontends:\n"
        "  - console\n"
        "  - gtk4\n"
        "capabilities:\n"
        "  - umicom.diagnostics\n"
        "  - umicom.configuration\n";

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK ||
        umi_fs_join(path,
                    sizeof(path),
                    temporary,
                    "umicom-application-manifest-test.yaml") != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_write_text(path, document) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_manifest_load(path, &manifest) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_application_manifest_validate(&manifest,
                                          message,
                                          sizeof(message)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(manifest.id, "org.umicom.designer") != 0 ||
        (manifest.frontends & UMI_FRONTEND_GTK4) == 0U ||
        manifest.capability_count != 2U ||
        !umi_application_manifest_has_capability(&manifest,
                                                 "umicom.diagnostics"))
        return EXIT_FAILURE;
    (void)remove(path);
    return EXIT_SUCCESS;
}
