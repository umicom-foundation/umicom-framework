/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/delivery_model/test_bundle_construction.c
 *
 * PURPOSE:
 *   Verify complete bundle replacement with borrowed inputs and controlled allocation failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/delivery/portable_bundle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned allocations, releases;
static size_t allocated_bytes;
static int refuse_allocation;
static void *BundleAllocate(size_t bytes)
{
    ++allocations; allocated_bytes = bytes;
    return refuse_allocation ? NULL : malloc(bytes);
}
static void BundleRelease(void *value) { ++releases; free(value); }
#define malloc BundleAllocate
#define free BundleRelease
#define umi_portable_bundle_init BundleInit
#define umi_portable_bundle_add_dependency BundleAddDependency
#define umi_portable_bundle_validate BundleValidate
#define umi_portable_bundle_file_count_checked BundleCountChecked
#define umi_portable_bundle_file_count BundleCount
#include "../../src/delivery/portable_bundle.c"
#undef umi_portable_bundle_file_count
#undef umi_portable_bundle_file_count_checked
#undef umi_portable_bundle_validate
#undef umi_portable_bundle_add_dependency
#undef umi_portable_bundle_init
#undef free
#undef malloc

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto done; \
} } while (0)

int main(int argc, char **argv)
{
    int result = 1;
    UmiPortableBundlePlan *bundle = malloc(sizeof *bundle);
    UmiPackageManifest *manifest = malloc(sizeof *manifest);
    unsigned char *before = malloc(sizeof *bundle);
    CHECK(argc == 2 && bundle != NULL && manifest != NULL && before != NULL);
    CHECK(umi_package_manifest_init(manifest, "example", "1.0", "bin/app", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
    CHECK(umi_package_manifest_add(manifest, "build/app", "bin/app", UMI_PACKAGE_FILE_EXECUTABLE, 1) == UMI_STATUS_OK);
    CHECK(BundleInit(bundle, "example", "windows-x64", "stage", "dist/app.zip", manifest) == UMI_STATUS_OK);
    CHECK(BundleValidate(bundle) == UMI_STATUS_OK);
    memcpy(before, bundle, sizeof *bundle);
    allocations = releases = 0U;
    const char *mode = argv[1];
    if (strcmp(mode, "borrowed") == 0) {
        CHECK(BundleInit(bundle, bundle->bundle_id, bundle->platform_id,
            bundle->staging_root, bundle->output_path, &bundle->manifest) == UMI_STATUS_OK);
        CHECK(BundleValidate(bundle) == UMI_STATUS_OK);
        CHECK(strcmp(bundle->bundle_id, "example") == 0 && strcmp(bundle->platform_id, "windows-x64") == 0);
        CHECK(strcmp(bundle->staging_root, "stage") == 0 && strcmp(bundle->output_path, "dist/app.zip") == 0);
        CHECK(bundle->manifest.count == 1U && strcmp(bundle->manifest.files[0].destination, "bin/app") == 0);
        CHECK(allocations == 1U && releases == 1U && allocated_bytes == sizeof *bundle);
    } else if (strcmp(mode, "allocation") == 0) {
        refuse_allocation = 1;
        CHECK(BundleInit(bundle, "replacement", "linux", "stage2", "out2", manifest) == UMI_STATUS_OUT_OF_MEMORY);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0 && allocations == 1U && releases == 0U);
        refuse_allocation = 0;
        CHECK(BundleInit(bundle, bundle->bundle_id, bundle->platform_id,
            bundle->staging_root, bundle->output_path, &bundle->manifest) == UMI_STATUS_OK);
        CHECK(BundleValidate(bundle) == UMI_STATUS_OK && bundle->manifest.count == 1U);
        CHECK(strcmp(bundle->bundle_id, "example") == 0 && allocations == 2U && releases == 1U);
    } else if (strcmp(mode, "null-inputs") == 0) {
        CHECK(BundleInit(NULL, "x", "y", "s", "o", manifest) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(allocations == 0U && releases == 0U);
        for (size_t i = 0U; i < 5U; ++i) {
            const char *arguments[] = {"replacement", "linux", "stage2", "out2"};
            if (i < 4U) arguments[i] = NULL;
            CHECK(BundleInit(bundle, arguments[0], arguments[1], arguments[2], arguments[3],
                i == 4U ? NULL : manifest) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        }
        CHECK(allocations == 5U && releases == 5U);
    } else if (strcmp(mode, "oversize-text") == 0) {
        char oversized[UMI_DELIVERY_PATH_CAPACITY + 1U];
        memset(oversized, 'x', sizeof oversized - 1U); oversized[sizeof oversized - 1U] = '\0';
        for (size_t i = 0U; i < 4U; ++i) {
            const char *arguments[] = {"replacement", "linux", "stage2", "out2"};
            arguments[i] = oversized;
            CHECK(BundleInit(bundle, arguments[0], arguments[1], arguments[2], arguments[3], manifest) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        }
        CHECK(allocations == 4U && releases == 4U);
    } else if (strcmp(mode, "invalid-manifest") == 0) {
        manifest->count = SIZE_MAX;
        CHECK(BundleInit(bundle, "replacement", "linux", "stage2", "out2", manifest) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0 && allocations == 1U && releases == 1U);
        manifest->count = 1U;
        memset(manifest->files[0].destination, 'x', sizeof manifest->files[0].destination);
        CHECK(BundleInit(bundle, "replacement", "linux", "stage2", "out2", manifest) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0 && allocations == 2U && releases == 2U);
    } else if (strcmp(mode, "draft") == 0) {
        memset(manifest, 0, sizeof *manifest);
        CHECK(BundleInit(bundle, "draft", "linux", "stage", "out", manifest) == UMI_STATUS_OK);
        CHECK(BundleValidate(bundle) == UMI_STATUS_INVALID_STATE);
        size_t count = 77U;
        CHECK(BundleCountChecked(bundle, &count) == UMI_STATUS_OK && count == 0U);
        CHECK(allocations == 1U && releases == 1U);
    } else CHECK(0);
    result = 0;
done:
    free(before); free(manifest); free(bundle);
    return result;
}
