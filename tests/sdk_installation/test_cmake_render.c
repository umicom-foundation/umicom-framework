/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_cmake_render.c
 *
 * PURPOSE:
 *   Verify literal SDK CMake arguments, exact sizing and complete output publication with controlled allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/sdk.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail_allocation;
static unsigned allocations, releases;
static void *RenderAllocate(size_t bytes)
{
    ++allocations;
    return fail_allocation ? NULL : malloc(bytes);
}
static void RenderRelease(void *value) { ++releases; free(value); }
#define malloc RenderAllocate
#define free RenderRelease
#define umi_sdk_consumer_package_init RenderPackageInit
#define umi_sdk_consumer_package_validate RenderPackageValidate
#define umi_sdk_consumer_package_cmake_size RenderPackageSize
#define umi_sdk_consumer_package_render_cmake RenderPackageText
#include "../../src/sdk/consumer_package.c"
#undef umi_sdk_consumer_package_render_cmake
#undef umi_sdk_consumer_package_cmake_size
#undef umi_sdk_consumer_package_validate
#undef umi_sdk_consumer_package_init
#undef free
#undef malloc

#define OUTPUT_CAPACITY 16384U
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); result = 1; goto cleanup; \
} } while (0)

typedef struct BadArgument { const char *mode; int field; const char *value; } BadArgument;
static const BadArgument bad_arguments[] = {
    {"package-command", 0, "Pkg)\nmessage(FATAL_ERROR injected)\n#"},
    {"package-space", 0, "Two Packages"}, {"package-list", 0, "A;B"},
    {"package-variable", 0, "${PACKAGE}"}, {"package-quote", 0, "A\"B"},
    {"package-path", 0, "../Package"}, {"package-escape", 0, "A\\B"},
    {"package-comment", 0, "A#comment"}, {"package-nonascii", 0, "Pack\xc3\xa9"},
    {"version-range", 1, "1...2"}, {"version-empty-part", 1, "1..2"},
    {"version-trailing", 1, "1."}, {"version-leading", 1, ".1"},
    {"version-five", 1, "1.2.3.4.5"}, {"version-text", 1, "1.2-beta"},
    {"version-option", 1, "1 REQUIRED"}, {"version-variable", 1, "${VERSION}"},
    {"target-command", 2, "T)\nmessage(FATAL_ERROR injected)\n#"},
    {"target-list", 2, "A;B"}, {"target-generator", 2, "$<TARGET_NAME:X>"},
    {"target-flag", 2, "-Wl,anything"}, {"target-path", 2, "lib/Thing"},
    {"target-private", 2, "PRIVATE"}, {"target-public", 2, "PUBLIC"},
    {"target-interface", 2, "INTERFACE"}, {"target-link-private", 2, "LINK_PRIVATE"},
    {"target-link-public", 2, "LINK_PUBLIC"}, {"target-link-interface", 2, "LINK_INTERFACE_LIBRARIES"},
    {"target-debug", 2, "debug"}, {"target-optimized", 2, "optimized"},
    {"target-general", 2, "general"}, {"target-single-colon", 2, "Vendor:Thing"},
    {"target-triple-colon", 2, "Vendor:::Thing"}, {"target-empty-namespace", 2, "::Thing"},
    {"target-empty-name", 2, "Vendor::"}
};

int main(int argc, char **argv)
{
    int result = 0;
    UmiSdkExportCatalogue *catalogue = malloc(sizeof *catalogue);
    char *output = malloc(OUTPUT_CAPACITY);
    unsigned char *before = malloc(OUTPUT_CAPACITY);
    CHECK(argc == 2 && catalogue != NULL && output != NULL && before != NULL);
    const char *mode = argv[1];
    UmiSdkConsumerPackage package;
    CHECK(RenderPackageInit(&package, "UmicomFramework", "Umicom", "0.9",
        "lib/cmake/UmicomFramework") == UMI_STATUS_OK);
    umi_sdk_export_catalogue_init(catalogue);
    CHECK(umi_sdk_export_catalogue_add(catalogue, "sdk", "Umicom::sdk", 1) == UMI_STATUS_OK);
    CHECK(umi_sdk_export_catalogue_add(catalogue, "delivery", "Umicom::delivery", 1) == UMI_STATUS_OK);
    allocations = releases = 0U;
    memset(output, 0xa5, OUTPUT_CAPACITY); memcpy(before, output, OUTPUT_CAPACITY);
    size_t bytes = SIZE_MAX, previous_size = bytes;
    int invalid_case = 0;
    for (size_t index = 0U; index < sizeof bad_arguments / sizeof bad_arguments[0]; ++index) {
        const BadArgument *bad = &bad_arguments[index];
        if (strcmp(mode, bad->mode) != 0) continue;
        char *field = bad->field == 0 ? package.package_name : bad->field == 1 ?
            package.minimum_version : catalogue->targets[1].target_name;
        size_t capacity = bad->field == 1 ? sizeof package.minimum_version : UMI_SDK_EXPORT_ID_CAPACITY;
        CHECK(strlen(bad->value) < capacity);
        strcpy(field, bad->value); invalid_case = 1; break;
    }
    if (strcmp(mode, "corrupt-count") == 0) { catalogue->count = SIZE_MAX; invalid_case = 1; }
    else if (strcmp(mode, "empty-catalogue") == 0) { catalogue->count = 0U; invalid_case = 1; }
    else if (strcmp(mode, "unterminated-package") == 0) {
        memset(package.package_name, 'x', sizeof package.package_name); invalid_case = 1;
    } else if (strcmp(mode, "unterminated-target") == 0) {
        memset(catalogue->targets[1].target_name, 'x', sizeof catalogue->targets[1].target_name); invalid_case = 1;
    }
    if (invalid_case) {
        CHECK(RenderPackageSize(&package, catalogue, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(bytes == previous_size);
        CHECK(RenderPackageText(&package, catalogue, output, OUTPUT_CAPACITY) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, output, OUTPUT_CAPACITY) == 0 && allocations == 0U);
    } else if (strcmp(mode, "null-inputs") == 0) {
        CHECK(RenderPackageSize(NULL, catalogue, &bytes) == UMI_STATUS_INVALID_ARGUMENT && bytes == previous_size);
        CHECK(RenderPackageSize(&package, NULL, &bytes) == UMI_STATUS_INVALID_ARGUMENT && bytes == previous_size);
        CHECK(RenderPackageSize(&package, catalogue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(RenderPackageText(NULL, catalogue, output, OUTPUT_CAPACITY) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(RenderPackageText(&package, NULL, output, OUTPUT_CAPACITY) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(RenderPackageText(&package, catalogue, NULL, OUTPUT_CAPACITY) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, output, OUTPUT_CAPACITY) == 0 && allocations == 0U);
    } else {
        const char *expected = "find_package(UmicomFramework 0.9 CONFIG REQUIRED)\n"
            "target_link_libraries(${PROJECT_NAME} PRIVATE Umicom::sdk Umicom::delivery)\n";
        if (strcmp(mode, "version-one") == 0) strcpy(package.minimum_version, "3");
        else if (strcmp(mode, "version-three") == 0) strcpy(package.minimum_version, "3.4.5");
        else if (strcmp(mode, "version-four") == 0) strcpy(package.minimum_version, "3.4.5.6");
        else if (strcmp(mode, "literal-names") == 0) {
            strcpy(package.package_name, "Package_2+extra-1.0");
            strcpy(catalogue->targets[0].target_name, "Vendor::Tool-v1.2+asm");
            strcpy(catalogue->targets[1].target_name, "plain_library");
        } else if (strcmp(mode, "full-catalogue") == 0) {
            for (size_t index = catalogue->count; index < UMI_SDK_EXPORT_CAPACITY; ++index) {
                char id[32], name[UMI_SDK_EXPORT_ID_CAPACITY];
                CHECK(snprintf(id, sizeof id, "component%zu", index) > 0);
                memset(name, 'x', sizeof name - 1U); name[sizeof name - 1U] = '\0';
                CHECK(umi_sdk_export_catalogue_add(catalogue, id, name, 1) == UMI_STATUS_OK);
            }
        } else CHECK(strcmp(mode, "baseline") == 0 || strcmp(mode, "short-capacities") == 0 ||
            strcmp(mode, "allocation") == 0 || strcmp(mode, "emit") == 0);
        fail_allocation = 1;
        CHECK(RenderPackageSize(&package, catalogue, &bytes) == UMI_STATUS_OK);
        CHECK(allocations == 0U && bytes < OUTPUT_CAPACITY);
        if (strcmp(mode, "allocation") == 0) {
            CHECK(RenderPackageText(&package, catalogue, output, bytes) == UMI_STATUS_OUT_OF_MEMORY);
            CHECK(memcmp(before, output, OUTPUT_CAPACITY) == 0 && allocations == 1U && releases == 0U);
        }
        fail_allocation = 0;
        if (strcmp(mode, "short-capacities") == 0) {
            for (size_t capacity = 0U; capacity < bytes; ++capacity) {
                CHECK(RenderPackageText(&package, catalogue, output, capacity) ==
                    (capacity == 0U ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED));
                CHECK(memcmp(before, output, OUTPUT_CAPACITY) == 0 && allocations == 0U);
            }
        }
        CHECK(RenderPackageText(&package, catalogue, output, bytes) == UMI_STATUS_OK);
        CHECK(strlen(output) + 1U == bytes);
        CHECK(memcmp(before + bytes, output + bytes, OUTPUT_CAPACITY - bytes) == 0);
        CHECK(releases == 1U && allocations == (strcmp(mode, "allocation") == 0 ? 2U : 1U));
        if (strcmp(mode, "baseline") == 0 || strcmp(mode, "emit") == 0) CHECK(strcmp(expected, output) == 0);
        if (strcmp(mode, "emit") == 0) {
            CHECK(fputs(output, stdout) >= 0);
            CHECK(fflush(stdout) == 0 && !ferror(stdout));
        }
    }
cleanup:
    free(before); free(output); free(catalogue);
    return result;
}
