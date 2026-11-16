/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_package_construction.c
 *
 * PURPOSE:
 *   Verify rejected package replacement, borrowed inputs and bounded fields using the production constructor with controlled allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/consumer_package.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail_allocation;
static unsigned allocations, releases;
static void *TestAllocate(size_t bytes)
{
    ++allocations;
    return fail_allocation ? NULL : malloc(bytes);
}
static void TestRelease(void *value)
{
    ++releases;
    free(value);
}
/* Include the production owner under private test names. Only allocation is
 * controlled; field copying and the shared staged-value wrapper remain real. */
#define malloc TestAllocate
#define free TestRelease
#define umi_sdk_consumer_package_init TestPackageInit
#define umi_sdk_consumer_package_validate TestPackageValidate
#define umi_sdk_consumer_package_render_cmake TestPackageRender
#define umi_sdk_consumer_package_cmake_size TestPackageSize
#include "../../src/sdk/consumer_package.c"
#undef umi_sdk_consumer_package_cmake_size
#undef umi_sdk_consumer_package_render_cmake
#undef umi_sdk_consumer_package_validate
#undef umi_sdk_consumer_package_init
#undef free
#undef malloc

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiSdkConsumerPackage package;
    CHECK(TestPackageInit(&package, "OldPackage", "OldNS", "3.4", "lib/cmake/Old") == UMI_STATUS_OK);
    package.include_examples = 0;
    package.include_toolchain_file = 0;
    unsigned char before[sizeof package];
    memcpy(before, &package, sizeof package);
    allocations = releases = 0U;
    if (strcmp(mode, "allocation") == 0) {
        fail_allocation = 1;
        CHECK(TestPackageInit(&package, "NewPackage", "NewNS", "4.0", "new/location") == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(allocations == 1U && releases == 0U);
        CHECK(memcmp(before, &package, sizeof package) == 0);
        fail_allocation = 0;
        CHECK(TestPackageInit(&package, "NewPackage", "NewNS", "4.0", "new/location") == UMI_STATUS_OK);
        CHECK(allocations == 2U && releases == 1U);
        CHECK(strcmp(package.package_name, "NewPackage") == 0);
    } else if (strcmp(mode, "null-output") == 0) {
        CHECK(TestPackageInit(NULL, "New", "NS", "1", "lib") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(allocations == 0U && releases == 0U);
    } else if (strcmp(mode, "borrow-all") == 0 || strcmp(mode, "borrow-cross") == 0) {
        int cross = strcmp(mode, "borrow-cross") == 0;
        CHECK(TestPackageInit(&package, cross ? package.namespace_name : package.package_name,
            cross ? package.package_name : package.namespace_name,
            package.minimum_version, package.config_directory) == UMI_STATUS_OK);
        CHECK(strcmp(package.package_name, cross ? "OldNS" : "OldPackage") == 0);
        CHECK(strcmp(package.namespace_name, cross ? "OldPackage" : "OldNS") == 0);
        CHECK(strcmp(package.minimum_version, "3.4") == 0);
        CHECK(strcmp(package.config_directory, "lib/cmake/Old") == 0);
        CHECK(package.include_examples == 1 && package.include_toolchain_file == 1);
        CHECK(allocations == 1U && releases == 1U);
    } else if (strcmp(mode, "validate-null") == 0) {
        CHECK(TestPackageValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strncmp(mode, "validate-", 9U) == 0) {
        char *field = NULL;
        size_t capacity = 0U;
        if (strcmp(mode, "validate-name") == 0) { field = package.package_name; capacity = sizeof package.package_name; }
        else if (strcmp(mode, "validate-namespace") == 0) { field = package.namespace_name; capacity = sizeof package.namespace_name; }
        else if (strcmp(mode, "validate-version") == 0) { field = package.minimum_version; capacity = sizeof package.minimum_version; }
        else { CHECK(strcmp(mode, "validate-directory") == 0); field = package.config_directory; capacity = sizeof package.config_directory; }
        memset(field, 'x', capacity);
        CHECK(TestPackageValidate(&package) == UMI_STATUS_INVALID_STATE);
        field[capacity - 1U] = '\0';
        CHECK(TestPackageValidate(&package) == UMI_STATUS_OK);
        field[0] = '\0';
        CHECK(TestPackageValidate(&package) == UMI_STATUS_INVALID_STATE);
    } else {
        const char *values[] = {"NewPackage", "NewNS", "4.0", "new/location"};
        size_t capacities[] = {sizeof package.package_name, sizeof package.namespace_name,
            sizeof package.minimum_version, sizeof package.config_directory};
        const char *suffix = strrchr(mode, '-');
        CHECK(suffix != NULL);
        size_t field = strcmp(suffix, "-name") == 0 ? 0U :
            strcmp(suffix, "-namespace") == 0 ? 1U : strcmp(suffix, "-version") == 0 ? 2U : 3U;
        CHECK(field != 3U || strcmp(suffix, "-directory") == 0);
        char oversized[UMI_SDK_PACKAGE_PATH_CAPACITY + 1U];
        UmiStatus expected;
        if (strncmp(mode, "null-", 5U) == 0) { values[field] = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (strncmp(mode, "empty-", 6U) == 0) { values[field] = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else {
            CHECK(strncmp(mode, "overflow-", 9U) == 0);
            memset(oversized, 'x', capacities[field]); oversized[capacities[field]] = '\0';
            values[field] = oversized; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        CHECK(TestPackageInit(&package, values[0], values[1], values[2], values[3]) == expected);
        CHECK(memcmp(before, &package, sizeof package) == 0);
        CHECK(allocations == 1U && releases == 1U);
    }
    return 0;
}
