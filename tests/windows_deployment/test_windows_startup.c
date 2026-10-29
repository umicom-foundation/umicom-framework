/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/windows_deployment/test_windows_startup.c
 *
 * PURPOSE:
 *   Verify the real Windows constructor updates the C runtime environment.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Verify exact package-relative paths, including the versioned overrides.
 * This fixture does not initialize GStreamer or claim audible playback. */
static int CheckMediaEnvironment(const char *resources)
{
    static const char suffix[] = "\\share\\umicom\\framework\\resources";
    static const char *pluginNames[] = {
        "GST_PLUGIN_PATH", "GST_PLUGIN_PATH_1_0",
        "GST_PLUGIN_SYSTEM_PATH", "GST_PLUGIN_SYSTEM_PATH_1_0"
    };
    static const char *scannerNames[] = {"GST_PLUGIN_SCANNER", "GST_PLUGIN_SCANNER_1_0"};
    char root[2048], plugins[2200], scanner[2200];
    size_t size = strlen(resources), suffixSize = sizeof(suffix) - 1U;
    if (size < suffixSize || strcmp(resources + size - suffixSize, suffix) != 0 ||
        size - suffixSize >= sizeof(root)) {
        fprintf(stderr, "Unexpected Framework resource layout.\n");
        return 0;
    }
    memcpy(root, resources, size - suffixSize);
    root[size - suffixSize] = '\0';
    int pluginSize = snprintf(plugins, sizeof(plugins), "%s\\lib\\gstreamer-1.0", root);
    int scannerSize = snprintf(scanner, sizeof(scanner),
        "%s\\libexec\\gstreamer-1.0\\gst-plugin-scanner.exe", root);
    if (pluginSize < 0 || (size_t)pluginSize >= sizeof(plugins) ||
        scannerSize < 0 || (size_t)scannerSize >= sizeof(scanner)) return 0;
    for (size_t i = 0U; i < sizeof(pluginNames) / sizeof(pluginNames[0]); ++i) {
        const char *value = getenv(pluginNames[i]);
        if (value == NULL || strcmp(value, plugins) != 0) {
            fprintf(stderr, "Media path did not replace inherited %s.\n", pluginNames[i]);
            return 0;
        }
    }
    for (size_t i = 0U; i < sizeof(scannerNames) / sizeof(scannerNames[0]); ++i) {
        const char *value = getenv(scannerNames[i]);
        if (value == NULL || strcmp(value, scanner) != 0) {
            fprintf(stderr, "Media scanner did not replace inherited %s.\n", scannerNames[i]);
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    const char *resources = getenv("UMICOM_FRAMEWORK_RESOURCE_ROOT");
    const char *cache = getenv("GDK_PIXBUF_MODULE_FILE");
    FILE *input;
    char buffer[2048];
    size_t size;
    if (resources == NULL || cache == NULL || strstr(resources, "framework") == NULL) {
        fprintf(stderr, "Native startup did not update the C runtime environment.\n");
        return EXIT_FAILURE;
    }
    if (!CheckMediaEnvironment(resources)) return EXIT_FAILURE;
    input = fopen(cache, "rb");
    if (input == NULL) {
        fprintf(stderr, "Relocated runtime cache was not readable.\n");
        return EXIT_FAILURE;
    }
    size = fread(buffer, 1U, sizeof(buffer) - 1U, input);
    buffer[size] = '\0';
    (void)fclose(input);
    if (strstr(buffer, "@UMICOM_RUNTIME_ROOT@") != NULL ||
        strstr(buffer, "fixture-loader.dll") == NULL) {
        fprintf(stderr, "Native cache relocation was incomplete.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
