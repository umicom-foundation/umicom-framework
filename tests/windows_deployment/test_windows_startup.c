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
