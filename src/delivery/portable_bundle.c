/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/portable_bundle.c
 *
 * PURPOSE:
 *   Plan a portable application bundle from a package and dependency manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/portable_bundle.h"
#include "delivery_internal.h"
#include <string.h>
#include "umicom/base/snapshot_validation.h"
#include "../base/value_construction_internal.h"

static UmiStatus PortableBundleStorage(const UmiPortableBundlePlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    static const UmiSnapshotTextField fields[] = {
        {"bundle_id", offsetof(UmiPortableBundlePlan, bundle_id), sizeof plan->bundle_id, 0},
        {"platform_id", offsetof(UmiPortableBundlePlan, platform_id), sizeof plan->platform_id, 0},
        {"staging_root", offsetof(UmiPortableBundlePlan, staging_root), sizeof plan->staging_root, 0},
        {"output_path", offsetof(UmiPortableBundlePlan, output_path), sizeof plan->output_path, 0}
    };
    if (UmiSnapshotValidateTextFields(plan, sizeof *plan, fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        umi_package_manifest_validate_storage(&plan->manifest) != UMI_STATUS_OK ||
        umi_runtime_dependency_report_validate(&plan->dependencies) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}


/*
 * Initialise portable bundle from caller-provided values so later operations receive a
 * known state.
 */
/* Portable bundle plans now validate complete nested storage before reading it and stage replacement before publishing. Checked counts separate invalid state from an empty plan. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_portable_bundle_init(UmiPortableBundlePlan *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifest *manifest)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || bundle_id == NULL || platform_id == NULL ||
        staging_root == NULL || output_path == NULL || manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->bundle_id,
                                    sizeof(plan->bundle_id), bundle_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->platform_id,
                                    sizeof(plan->platform_id), platform_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->staging_root,
                                    sizeof(plan->staging_root), staging_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->output_path,
                                    sizeof(plan->output_path), output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->manifest = *manifest;
    umi_runtime_dependency_report_init(&plan->dependencies);
    return UMI_STATUS_OK;
}

#endif
static UmiStatus PortableBundleBuild(UmiPortableBundlePlan *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifest *manifest)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || bundle_id == NULL || platform_id == NULL ||
        staging_root == NULL || output_path == NULL || manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_package_manifest_validate_storage(manifest) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->bundle_id,
                                    sizeof(plan->bundle_id), bundle_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->platform_id,
                                    sizeof(plan->platform_id), platform_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->staging_root,
                                    sizeof(plan->staging_root), staging_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->output_path,
                                    sizeof(plan->output_path), output_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->manifest = *manifest;
    umi_runtime_dependency_report_init(&plan->dependencies);
    return UMI_STATUS_OK;
}


UMI_DEFINE_STAGED_VALUE_INIT(umi_portable_bundle_init, UmiPortableBundlePlan, plan,
    (UmiPortableBundlePlan *plan, const char *bundle_id, const char *platform_id,
     const char *staging_root, const char *output_path, const UmiPackageManifest *manifest),
    PortableBundleBuild(candidate, bundle_id, platform_id, staging_root, output_path, manifest))

/*
 * Provide the portable bundle add dependency operation used by this module and its client
 * applications.
 */
UmiStatus umi_portable_bundle_add_dependency(
    UmiPortableBundlePlan *plan,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_runtime_dependency_report_add(&plan->dependencies, name,
                                                 resolved_path, disposition,
                                                 resolved);
}

/* Check that portable bundle satisfies its contract before another service relies on it. */
/* Portable bundle plans now validate complete nested storage before reading it and stage replacement before publishing. Checked counts separate invalid state from an empty plan. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_portable_bundle_validate(const UmiPortableBundlePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (plan->bundle_id[0] == '\0' || plan->platform_id[0] == '\0' ||
        plan->staging_root[0] == '\0' || plan->output_path[0] == '\0' ||
        umi_package_manifest_validate(&plan->manifest) != UMI_STATUS_OK ||
        !umi_runtime_dependency_report_ready(&plan->dependencies)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_portable_bundle_validate(const UmiPortableBundlePlan *plan)
{
    UmiStatus status = PortableBundleStorage(plan);
    if (status != UMI_STATUS_OK) return status;
    if (plan->bundle_id[0] == '\0' || plan->platform_id[0] == '\0' ||
        plan->staging_root[0] == '\0' || plan->output_path[0] == '\0' ||
        umi_package_manifest_validate(&plan->manifest) != UMI_STATUS_OK ||
        !umi_runtime_dependency_report_ready(&plan->dependencies))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by portable bundle file without changing their
 * state.
 */
/* Portable bundle plans now validate complete nested storage before reading it and stage replacement before publishing. Checked counts separate invalid state from an empty plan. The former implementation is retained for owner review. */
#if 0
size_t umi_portable_bundle_file_count(const UmiPortableBundlePlan *plan)
{
    size_t count;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return 0U;
    count = plan->manifest.count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->dependencies.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (plan->dependencies.dependencies[index].disposition ==
            UMI_DEPENDENCY_BUNDLE) ++count;
    }
    return count;
}

#endif
UmiStatus umi_portable_bundle_file_count_checked(const UmiPortableBundlePlan *plan, size_t *out_count)
{
    if (out_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PortableBundleStorage(plan);
    if (status != UMI_STATUS_OK) return status;
    size_t count = plan->manifest.count;
    for (size_t i = 0U; i < plan->dependencies.count; ++i) {
        if (plan->dependencies.dependencies[i].disposition == UMI_DEPENDENCY_BUNDLE) {
            if (count == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++count;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}

size_t umi_portable_bundle_file_count(const UmiPortableBundlePlan *plan)
{
    if (plan == NULL) return 0U; /* Retain the established null-input result. */
    size_t count;
    return umi_portable_bundle_file_count_checked(plan, &count) == UMI_STATUS_OK ? count : SIZE_MAX;
}