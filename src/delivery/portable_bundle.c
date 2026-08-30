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

UmiStatus umi_portable_bundle_init(UmiPortableBundlePlan *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifest *manifest)
{
    UmiStatus status;
    if (plan == NULL || bundle_id == NULL || platform_id == NULL ||
        staging_root == NULL || output_path == NULL || manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->bundle_id,
                                    sizeof(plan->bundle_id), bundle_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->platform_id,
                                    sizeof(plan->platform_id), platform_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->staging_root,
                                    sizeof(plan->staging_root), staging_root);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->output_path,
                                    sizeof(plan->output_path), output_path);
    if (status != UMI_STATUS_OK) return status;
    plan->manifest = *manifest;
    umi_runtime_dependency_report_init(&plan->dependencies);
    return UMI_STATUS_OK;
}

UmiStatus umi_portable_bundle_add_dependency(
    UmiPortableBundlePlan *plan,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_runtime_dependency_report_add(&plan->dependencies, name,
                                                 resolved_path, disposition,
                                                 resolved);
}

UmiStatus umi_portable_bundle_validate(const UmiPortableBundlePlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->bundle_id[0] == '\0' || plan->platform_id[0] == '\0' ||
        plan->staging_root[0] == '\0' || plan->output_path[0] == '\0' ||
        umi_package_manifest_validate(&plan->manifest) != UMI_STATUS_OK ||
        !umi_runtime_dependency_report_ready(&plan->dependencies)) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

size_t umi_portable_bundle_file_count(const UmiPortableBundlePlan *plan)
{
    size_t count;
    size_t index;
    if (plan == NULL) return 0U;
    count = plan->manifest.count;
    for (index = 0U; index < plan->dependencies.count; ++index) {
        if (plan->dependencies.dependencies[index].disposition ==
            UMI_DEPENDENCY_BUNDLE) ++count;
    }
    return count;
}
