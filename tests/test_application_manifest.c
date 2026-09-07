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
#include "umicom/runtime/application_manifest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Use the real parser with in-memory text: malformed-name regressions neither
 * execute a program nor touch a personal or fixed temporary manifest file. */
static int check_launch_declarations(void)
{
    static const char prefix[] =
        "schema: umicom.application.v1\napplication:\n"
        "  id: org.umicom.external-test\n  name: Test\n  version: 1.0\n"
        "  executable: legacy-name\n";
    static const char suffix[] =
        "framework:\n  minimum_version: 0.9.0\n"
        "frontends:\n  - console\n  - gtk4\n";
    static const char *const invalid[] = {
        "  native_executable: ../tool\n",
        "  native_executable: C:\\tool.exe\n",
        "  native_executable: tool --argument\n",
        "  native_executable: tool;other\n",
        "  native_executable: tool|other\n",
        "  native_executable: $(tool)\n",
        "  native_executable: %TOOL%\n",
        "  native_executable: \"tool\"\n",
        "  native_executable: -tool\n",
        "  native_executable: tool.\n",
        "  native_executable: \n",
        "  native_executable: tool\n  native_executable: other\n",
        "  console_executable: tool\n  console_executable: other\n",
        "  executable: duplicate-legacy\n",
        "  console_executable: ../console\n",
        "  console_executable: \n",
        "  native_executable tool\n",
        "  metadata:\n    native_executable: unexpected-tool\n",
        "    native_executable: wrong-indent\n",
        "schema: umicom.application/v1\n"
    };
    UmiApplicationManifest *manifest = calloc(1U, sizeof(*manifest));
    UmiApplicationLaunchSpec launch_spec;
    char text[2048];
    char long_name[UMI_MANIFEST_TEXT_CAPACITY + 8U];
    int success = 1;
    if (manifest == NULL) return 0;
    (void)snprintf(text, sizeof(text), "%s%s", prefix, suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_OK;
    success &= launch_spec.native_executable[0] == '\0' &&
        launch_spec.console_executable[0] == '\0' &&
        strcmp(manifest->executable, "legacy-name") == 0;
    (void)snprintf(text, sizeof(text), "%s%s%s", prefix,
        "  native_executable: product-gui\n  console_executable: product-console\n", suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_OK;
    success &= strcmp(launch_spec.native_executable, "product-gui") == 0 &&
        strcmp(launch_spec.console_executable, "product-console") == 0 &&
        strcmp(manifest->executable, "legacy-name") == 0;

    /* A failed parse must clear earlier successful names, not leak stale data. */
    for (size_t index = 0U; index < sizeof(invalid) / sizeof(invalid[0]); ++index) {
        (void)snprintf(text, sizeof(text), "%s%s%s", prefix, invalid[index], suffix);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_PARSE_ERROR;
        success &= manifest->id[0] == '\0' && launch_spec.native_executable[0] == '\0' &&
            launch_spec.console_executable[0] == '\0';
    }
    (void)memset(long_name, 'x', sizeof(long_name) - 1U);
    long_name[sizeof(long_name) - 1U] = '\0';
    (void)snprintf(text, sizeof(text), "%s  native_executable: %s\n%s",
        prefix, long_name, suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_CAPACITY_EXCEEDED;
    success &= manifest->id[0] == '\0' && launch_spec.native_executable[0] == '\0';

    /* Explicit targets require the matching advertised frontend. */
    (void)snprintf(text, sizeof(text), "%s  native_executable: tool\nfrontends:\n  - console\n", prefix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_PARSE_ERROR;
    (void)snprintf(text, sizeof(text), "%s  console_executable: tool\nfrontends:\n  - gtk4\n", prefix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_PARSE_ERROR;

    success &= umi_application_manifest_parse_with_launch_spec(
        "application:\n  id: earlier\nschema: umicom.application/v1\n",
        manifest, &launch_spec) == UMI_STATUS_PARSE_ERROR;

    /* Exact existing developer-project shape uses flat /v1, gtk4 and
     * minimum_framework_version. Its spelling and legacy executable survive. */
    {
        static const char flat[] =
            "schema: umicom.application/v1\n"
            "id: org.umicom.generated\nname: Generated\nshort_name: generated\n"
            "version: 0.1.0\nexecutable: generated\n"
            "minimum_framework_version: 0.9.0\n";
        static const char flat_frontends[] =
            "frontends:\n  - console\n  - gtk4\ncapabilities:\n"
            "  - umicom.runtime\n  - umicom.application\n";
        (void)snprintf(text, sizeof(text), "%s%s", flat, flat_frontends);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_OK;
        success &= strcmp(manifest->schema, "umicom.application/v1") == 0 &&
            strcmp(manifest->id, "org.umicom.generated") == 0 &&
            strcmp(manifest->minimum_framework_version, "0.9.0") == 0 &&
            strcmp(manifest->executable, "generated") == 0 &&
            launch_spec.native_executable[0] == '\0';
        (void)snprintf(text, sizeof(text), "%s%s%s", flat,
            "native_executable: generated-gui\nconsole_executable: generated-cli\n",
            flat_frontends);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_OK && strcmp(launch_spec.native_executable, "generated-gui") == 0;
        (void)snprintf(text, sizeof(text), "%s%s%s", flat,
            "native_executable: first\nnative_executable: second\n", flat_frontends);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_PARSE_ERROR;
        (void)snprintf(text, sizeof(text), "%s%s%s", flat,
            "application:\n  native_executable: nested\n", flat_frontends);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_PARSE_ERROR;
        (void)snprintf(text, sizeof(text), "%s%s%s", flat,
            "  native_executable: indented\n", flat_frontends);
        success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
            == UMI_STATUS_PARSE_ERROR;
    }
    (void)snprintf(text, sizeof(text), "%snative_executable: flat\n%s", prefix, suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_PARSE_ERROR;
    (void)snprintf(text, sizeof(text), "%sunknown_metadata: ignored\n"
        "  native_executable: ignored\n%s", prefix, suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_OK && launch_spec.native_executable[0] == '\0';

    /* Unrelated sections cannot overwrite identity or silently add launch names. */
    (void)snprintf(text, sizeof(text), "%s%sentry:\n  native_executable: ignored\n", prefix, suffix);
    success &= umi_application_manifest_parse_with_launch_spec(text, manifest, &launch_spec)
        == UMI_STATUS_OK && launch_spec.native_executable[0] == '\0';
    (void)memset(launch_spec.native_executable, 'x', sizeof(launch_spec.native_executable));
    success &= umi_application_launch_spec_validate(manifest, &launch_spec)
        == UMI_STATUS_PARSE_ERROR;
    umi_application_launch_spec_init(&launch_spec);
    launch_spec.structure_size = 0U;
    success &= umi_application_launch_spec_validate(manifest, &launch_spec)
        == UMI_STATUS_INVALID_ARGUMENT;
    if (!success) (void)fprintf(stderr, "Explicit frontend launch declaration regression failed\n");
    free(manifest);
    return success;
}

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

    if (!check_launch_declarations()) return EXIT_FAILURE;
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
