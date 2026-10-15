/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_modules/test_resources.c
 * PURPOSE: Exercise template lookup decisions without depending on a machine installation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_module.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define RESOURCE_PREFIX "C:/umicom resource fixture"
#define RESOURCE_SOURCE "C:/umicom source fixture/templates/application_module"
#else
#define RESOURCE_PREFIX "/umicom resource fixture"
#define RESOURCE_SOURCE "/umicom source fixture/templates/application_module"
#endif
static int installed_available, source_available, image_available = 1;
static UmiStatus ResourceImage(char *out, size_t capacity)
{
    if (!image_available) return UMI_STATUS_IO_ERROR;
    return umi_path_copy(out, capacity, RESOURCE_PREFIX "/bin/studio");
}
static int ResourceDirectory(const char *path)
{
    return (installed_available && umi_path_equal(path,
        RESOURCE_PREFIX "/share/umicom/templates/application_module")) ||
        (source_available && umi_path_equal(path, RESOURCE_SOURCE));
}
/* Replace only environmental discovery. Production path construction and output
 * publication remain unchanged, making missing resources reproducible. */
#define umi_fs_executable_path ResourceImage
#define umi_fs_is_directory ResourceDirectory
#define umi_application_scaffold_template_root ResourceLocate
#define UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT RESOURCE_SOURCE
#define UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR "share/umicom/templates/application_module"
#include "../../src/scaffold/application_resources.c"
#undef umi_fs_executable_path
#undef umi_fs_is_directory
#undef umi_application_scaffold_template_root
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    char output[UMI_PATH_CAPACITY] = "unchanged";
    CHECK(argc == 2);
    if (strcmp(argv[1], "installed") == 0) installed_available = source_available = 1;
    else if (strcmp(argv[1], "fallback") == 0) source_available = 1;
    else if (strcmp(argv[1], "image_failure") == 0) { image_available = 0; source_available = 1; }
    else if (strcmp(argv[1], "capacity") == 0) {
        installed_available = 1;
        CHECK(ResourceLocate(output, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        return 0;
    } else if (strcmp(argv[1], "null") == 0) {
        CHECK(ResourceLocate(NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ResourceLocate(output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "unchanged") == 0);
        return 0;
    } else if (strcmp(argv[1], "missing") != 0) return 2;
    UmiStatus status = ResourceLocate(output, sizeof output);
    if (!installed_available && !source_available) {
        CHECK(status == UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(output, "unchanged") == 0);
    } else {
        CHECK(status == UMI_STATUS_OK);
        CHECK(umi_path_equal(output, installed_available
            ? RESOURCE_PREFIX "/share/umicom/templates/application_module" : RESOURCE_SOURCE));
    }
    return 0;
}
