/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/delivery_model/test_integrity.c
 *
 * PURPOSE:
 *   Verify portable delivery model bounds, append publication and checked counts without filesystem work.
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

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto done; \
} } while (0)

static UmiStatus Prepare(UmiPortableBundlePlan *bundle)
{
    UmiStatus status = umi_package_manifest_init(&bundle->manifest, "example", "1.0", "bin/app", UMI_PACKAGE_ZIP);
    if (status == UMI_STATUS_OK)
        status = umi_package_manifest_add(&bundle->manifest, "build/app", "bin/app", UMI_PACKAGE_FILE_EXECUTABLE, 1);
    if (status == UMI_STATUS_OK)
        status = umi_portable_bundle_init(bundle, "example-portable", "windows-x64", "stage", "output.zip", &bundle->manifest);
    return status;
}

int main(int argc, char **argv)
{
    int result = 1;
    UmiPortableBundlePlan *bundle = calloc(1U, sizeof *bundle);
    unsigned char *before = malloc(sizeof *bundle);
    CHECK(argc == 2 && bundle != NULL && before != NULL);
    UmiPackageManifest *manifest = &bundle->manifest;
    UmiRuntimeDependencyReport *report = &bundle->dependencies;
    const char *mode = argv[1];
    CHECK(Prepare(bundle) == UMI_STATUS_OK);
    if (strcmp(mode, "manifest-storage") == 0) {
        CHECK(umi_package_manifest_validate_storage(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_package_manifest_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        const size_t bad_counts[] = {UMI_DELIVERY_MAX_ARTIFACTS + 1U, SIZE_MAX};
        for (size_t i = 0U; i < sizeof bad_counts / sizeof bad_counts[0]; ++i) {
            manifest->count = bad_counts[i]; memcpy(before, bundle, sizeof *bundle);
            CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_package_manifest_find(manifest, "bin/app") == NULL);
            CHECK(umi_package_manifest_add(manifest, "src", "new", UMI_PACKAGE_FILE_RESOURCE, 1) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        }
        CHECK(Prepare(bundle) == UMI_STATUS_OK);
        char *fields[] = {manifest->product_id, manifest->version, manifest->entrypoint,
            manifest->files[0].source, manifest->files[0].destination};
        const size_t capacities[] = {sizeof manifest->product_id, sizeof manifest->version,
            sizeof manifest->entrypoint, sizeof manifest->files[0].source, sizeof manifest->files[0].destination};
        for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
            CHECK(Prepare(bundle) == UMI_STATUS_OK);
            memset(fields[i], 'x', capacities[i]); memcpy(before, bundle, sizeof *bundle);
            CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_package_manifest_find(manifest, "bin/app") == NULL);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
            fields[i][capacities[i] - 1U] = '\0';
            CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_OK);
        }
        CHECK(Prepare(bundle) == UMI_STATUS_OK);
        manifest->format = (UmiPackageFormat)0;
        CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_OK);
        CHECK(umi_package_manifest_validate(manifest) == UMI_STATUS_INVALID_STATE);
        manifest->files[0].role = (UmiPackageFileRole)99;
        CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "manifest-append") == 0) {
        CHECK(umi_package_manifest_init(manifest, "example", "1.0", "bin/app", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
        strcpy(manifest->files[0].source, "build/app"); strcpy(manifest->files[0].destination, "bin/app");
        CHECK(umi_package_manifest_add(manifest, manifest->files[0].source,
            manifest->files[0].destination, UMI_PACKAGE_FILE_EXECUTABLE, 1) == UMI_STATUS_OK);
        CHECK(strcmp(manifest->files[0].source, "build/app") == 0);
        CHECK(strcmp(manifest->files[0].destination, "bin/app") == 0);
        memset(&manifest->files[1], 0xa5, sizeof manifest->files[1]);
        char oversized[UMI_DELIVERY_PATH_CAPACITY + 1U];
        memset(oversized, 'x', sizeof oversized - 1U); oversized[sizeof oversized - 1U] = '\0';
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_package_manifest_add(manifest, "src", oversized, UMI_PACKAGE_FILE_RESOURCE, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        CHECK(umi_package_manifest_add(manifest, oversized, "new", UMI_PACKAGE_FILE_RESOURCE, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        CHECK(umi_package_manifest_add(manifest, "src", "new", (UmiPackageFileRole)0, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        CHECK(umi_package_manifest_add(manifest, manifest->files[0].source, "bin/copy", UMI_PACKAGE_FILE_RESOURCE, 0) == UMI_STATUS_OK);
        CHECK(manifest->count == 2U && strcmp(manifest->files[1].source, "build/app") == 0);
    } else if (strcmp(mode, "manifest-capacity") == 0) {
        CHECK(umi_package_manifest_init(manifest, "example", "1.0", "bin/0", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
        for (size_t i = 0U; i < UMI_DELIVERY_MAX_ARTIFACTS; ++i) {
            char path[32]; int n = snprintf(path, sizeof path, "bin/%zu", i);
            CHECK(n > 0 && (size_t)n < sizeof path);
            CHECK(umi_package_manifest_add(manifest, path, path, UMI_PACKAGE_FILE_EXECUTABLE, 1) == UMI_STATUS_OK);
        }
        CHECK(umi_package_manifest_validate(manifest) == UMI_STATUS_OK);
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_package_manifest_add(manifest, "src", "extra", UMI_PACKAGE_FILE_RESOURCE, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        memset(manifest->files[manifest->count - 1U].destination, 'x', UMI_DELIVERY_PATH_CAPACITY);
        CHECK(umi_package_manifest_find(manifest, "bin/0") == NULL);
    } else if (strcmp(mode, "manifest-duplicates") == 0) {
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_package_manifest_add(manifest, "different", "bin/app", UMI_PACKAGE_FILE_RESOURCE, 1) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        manifest->files[1] = manifest->files[0]; manifest->count = 2U;
        CHECK(umi_package_manifest_validate_storage(manifest) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_package_manifest_find(manifest, "bin/app") == NULL);
    } else if (strcmp(mode, "dependency-storage") == 0) {
        size_t count = 77U;
        CHECK(umi_runtime_dependency_report_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_runtime_dependency_report_unresolved_count_checked(NULL, &count) == UMI_STATUS_INVALID_ARGUMENT && count == 77U);
        CHECK(umi_runtime_dependency_report_unresolved_count(NULL) == 0U);
        CHECK(!umi_runtime_dependency_report_ready(NULL));
        report->count = SIZE_MAX;
        CHECK(umi_runtime_dependency_report_unresolved_count_checked(report, &count) == UMI_STATUS_INVALID_STATE && count == 77U);
        CHECK(umi_runtime_dependency_report_unresolved_count(report) == SIZE_MAX);
        CHECK(!umi_runtime_dependency_report_ready(report));
        umi_runtime_dependency_report_init(report);
        CHECK(umi_runtime_dependency_report_add(report, "one", "lib/one", UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_OK);
        for (size_t field = 0U; field < 2U; ++field) {
            UmiRuntimeDependency saved = report->dependencies[0];
            char *text = field == 0U ? report->dependencies[0].name : report->dependencies[0].resolved_path;
            size_t capacity = field == 0U ? sizeof saved.name : sizeof saved.resolved_path;
            memset(text, 'x', capacity); memcpy(before, bundle, sizeof *bundle);
            CHECK(umi_runtime_dependency_report_validate(report) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_runtime_dependency_report_unresolved_count_checked(report, &count) == UMI_STATUS_INVALID_STATE && count == 77U);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
            text[capacity - 1U] = '\0';
            CHECK(umi_runtime_dependency_report_validate(report) == UMI_STATUS_OK);
            report->dependencies[0] = saved;
        }
        report->dependencies[0].disposition = (UmiDependencyDisposition)99;
        CHECK(umi_runtime_dependency_report_validate(report) == UMI_STATUS_INVALID_STATE);
        report->dependencies[0].disposition = UMI_DEPENDENCY_BUNDLE;
        report->dependencies[0].resolved_path[0] = '\0';
        CHECK(!umi_runtime_dependency_report_ready(report));
    } else if (strcmp(mode, "dependency-append") == 0) {
        strcpy(report->dependencies[0].name, "one"); strcpy(report->dependencies[0].resolved_path, "lib/one");
        CHECK(umi_runtime_dependency_report_add(report, report->dependencies[0].name,
            report->dependencies[0].resolved_path, UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_OK);
        CHECK(strcmp(report->dependencies[0].resolved_path, "lib/one") == 0);
        memset(&report->dependencies[1], 0xa5, sizeof report->dependencies[1]);
        char oversized[UMI_DELIVERY_PATH_CAPACITY + 1U];
        memset(oversized, 'x', sizeof oversized - 1U); oversized[sizeof oversized - 1U] = '\0';
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_runtime_dependency_report_add(report, "two", oversized, UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        CHECK(umi_runtime_dependency_report_add(report, "two", "", UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_runtime_dependency_report_add(report, "two", "", (UmiDependencyDisposition)0, 0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        CHECK(umi_runtime_dependency_report_add(report, "two", report->dependencies[0].resolved_path, UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_OK);
    } else if (strcmp(mode, "dependency-capacity") == 0) {
        for (size_t i = 0U; i < UMI_DELIVERY_MAX_COMPONENTS; ++i) {
            char name[32]; int n = snprintf(name, sizeof name, "library-%zu", i);
            CHECK(n > 0 && (size_t)n < sizeof name);
            CHECK(umi_runtime_dependency_report_add(report, name, "", UMI_DEPENDENCY_SYSTEM, 0) == UMI_STATUS_OK);
        }
        CHECK(umi_runtime_dependency_report_ready(report));
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_runtime_dependency_report_add(report, "extra", "", UMI_DEPENDENCY_SYSTEM, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        report->count = UMI_DELIVERY_MAX_COMPONENTS + 1U;
        CHECK(!umi_runtime_dependency_report_ready(report));
    } else if (strcmp(mode, "dependency-duplicates") == 0) {
        CHECK(umi_runtime_dependency_report_add(report, "one", "", UMI_DEPENDENCY_SYSTEM, 0) == UMI_STATUS_OK);
        memcpy(before, bundle, sizeof *bundle);
        CHECK(umi_runtime_dependency_report_add(report, "one", "other", UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        report->dependencies[1] = report->dependencies[0]; report->count = 2U;
        CHECK(umi_runtime_dependency_report_validate(report) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "bundle-count") == 0) {
        size_t count = 77U;
        CHECK(umi_portable_bundle_file_count_checked(bundle, &count) == UMI_STATUS_OK && count == 1U);
        CHECK(umi_runtime_dependency_report_add(report, "system", "", UMI_DEPENDENCY_SYSTEM, 0) == UMI_STATUS_OK);
        CHECK(umi_runtime_dependency_report_add(report, "excluded", "", UMI_DEPENDENCY_EXCLUDE, 0) == UMI_STATUS_OK);
        CHECK(umi_runtime_dependency_report_add(report, "pending", "", UMI_DEPENDENCY_BUNDLE, 0) == UMI_STATUS_OK);
        CHECK(umi_portable_bundle_file_count_checked(bundle, &count) == UMI_STATUS_OK && count == 2U);
        CHECK(umi_runtime_dependency_report_unresolved_count_checked(report, &count) == UMI_STATUS_OK && count == 1U);
        CHECK(umi_portable_bundle_validate(bundle) == UMI_STATUS_INVALID_STATE);
        strcpy(report->dependencies[2].resolved_path, "lib/pending"); report->dependencies[2].resolved = 1;
        CHECK(umi_portable_bundle_validate(bundle) == UMI_STATUS_OK);
        CHECK(umi_portable_bundle_file_count(bundle) == 2U);
        CHECK(umi_runtime_dependency_report_unresolved_count_checked(report, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_portable_bundle_file_count_checked(bundle, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_portable_bundle_file_count_checked(NULL, &count) == UMI_STATUS_INVALID_ARGUMENT && count == 1U);
        CHECK(umi_portable_bundle_file_count(NULL) == 0U);
        memset(bundle, 0, sizeof *bundle);
        CHECK(umi_portable_bundle_file_count_checked(bundle, &count) == UMI_STATUS_OK && count == 0U);
        CHECK(umi_portable_bundle_validate(bundle) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "bundle-invalid") == 0) {
        char *fields[] = {bundle->bundle_id, bundle->platform_id, bundle->staging_root, bundle->output_path};
        const size_t capacities[] = {sizeof bundle->bundle_id, sizeof bundle->platform_id,
            sizeof bundle->staging_root, sizeof bundle->output_path};
        for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
            CHECK(Prepare(bundle) == UMI_STATUS_OK);
            memset(fields[i], 'x', capacities[i]);
            size_t count = 77U; memcpy(before, bundle, sizeof *bundle);
            CHECK(umi_portable_bundle_file_count_checked(bundle, &count) == UMI_STATUS_INVALID_STATE && count == 77U);
            CHECK(umi_portable_bundle_validate(bundle) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_portable_bundle_file_count(bundle) == SIZE_MAX);
            CHECK(memcmp(before, bundle, sizeof *bundle) == 0);
        }
        CHECK(Prepare(bundle) == UMI_STATUS_OK); manifest->count = SIZE_MAX;
        CHECK(umi_portable_bundle_file_count(bundle) == SIZE_MAX);
        CHECK(Prepare(bundle) == UMI_STATUS_OK); report->count = SIZE_MAX;
        CHECK(umi_portable_bundle_file_count(bundle) == SIZE_MAX);
    } else CHECK(0);
    result = 0;
done:
    free(before); free(bundle);
    return result;
}
