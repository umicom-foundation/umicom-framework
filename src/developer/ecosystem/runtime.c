/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/runtime.c
 * PURPOSE: Implement the Framework-owned developer ecosystem coordinator.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/runtime.h"

#include <stdlib.h>
#include <string.h>

struct UmiEcosystemRuntime {
    UmiEcosystemSourceCatalogue sources;
    UmiEcosystemPackageCatalogue packages;
    UmiEcosystemSdkInventory sdks;
    UmiEcosystemComponentWizard components;
    uint64_t revision;
};

/*
 * Initialise ecosystem runtime from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ecosystem_runtime_create(UmiEcosystemRuntime **out_runtime)
{
    UmiEcosystemRuntime *runtime;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiEcosystemRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_ecosystem_source_catalogue_init(&runtime->sources);
    umi_ecosystem_package_catalogue_init(&runtime->packages);
    umi_ecosystem_sdk_inventory_init(&runtime->sdks);
    umi_ecosystem_component_wizard_init(&runtime->components);
    runtime->revision = 1U;
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ecosystem runtime so the same storage can be reused
 * safely.
 */
void umi_ecosystem_runtime_destroy(UmiEcosystemRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return;
    (void)memset(runtime, 0, sizeof(*runtime));
    free(runtime);
}

/*
 * Provide the ecosystem runtime sources operation used by this module and its client
 * applications.
 */
UmiEcosystemSourceCatalogue *umi_ecosystem_runtime_sources(
    UmiEcosystemRuntime *runtime)
{
    return runtime != NULL ? &runtime->sources : NULL;
}

/*
 * Provide the ecosystem runtime packages operation used by this module and its client
 * applications.
 */
UmiEcosystemPackageCatalogue *umi_ecosystem_runtime_packages(
    UmiEcosystemRuntime *runtime)
{
    return runtime != NULL ? &runtime->packages : NULL;
}

/*
 * Provide the ecosystem runtime sdks operation used by this module and its client
 * applications.
 */
UmiEcosystemSdkInventory *umi_ecosystem_runtime_sdks(
    UmiEcosystemRuntime *runtime)
{
    return runtime != NULL ? &runtime->sdks : NULL;
}

/*
 * Provide the ecosystem runtime components operation used by this module and its client
 * applications.
 */
UmiEcosystemComponentWizard *umi_ecosystem_runtime_components(
    UmiEcosystemRuntime *runtime)
{
    return runtime != NULL ? &runtime->components : NULL;
}

/*
 * Provide the ecosystem runtime set offline operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_set_offline(
    UmiEcosystemRuntime *runtime,
    bool offline_mode)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_source_catalogue_set_offline(
        &runtime->sources, offline_mode);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision++;
    return status;
}

/*
 * Provide the ecosystem runtime query operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_query(
    const UmiEcosystemRuntime *runtime,
    const UmiEcosystemPackageQuery *query,
    UmiEcosystemPackageQueryResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ecosystem_package_query_execute(
        &runtime->packages, &runtime->sources, query, out_result);
}

/*
 * Provide the add install action operation used by this module and its client
 * applications.
 */
static UmiStatus add_install_action(
    UmiEcosystemInstallReview *review,
    UmiEcosystemActionKind kind,
    const char *summary,
    UmiEcosystemReviewSeverity severity,
    bool requires_approval)
{
    return umi_ecosystem_install_review_add_action(review, kind,
        review->package_id, summary, severity, true, requires_approval);
}

/*
 * Provide the ecosystem runtime prepare install operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_runtime_prepare_install(
    const UmiEcosystemRuntime *runtime,
    const char *package_id,
    UmiEcosystemInstallReview *out_review)
{
    const UmiEcosystemPackageRecord *record;
    const UmiEcosystemSource *source;
    UmiStatus status;
    UmiEcosystemReviewSeverity evidence_severity;
    bool evidence_approval;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || package_id == NULL || out_review == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    record = umi_ecosystem_package_catalogue_find_const(
        &runtime->packages, package_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_ecosystem_install_review_begin(out_review, record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    source = umi_ecosystem_source_catalogue_find_const(
        &runtime->sources, record->source_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL ||
        !umi_ecosystem_source_available(&runtime->sources, source)) {
        status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_DOWNLOAD,
            "The selected source is unavailable in the current connectivity mode.",
            UMI_ECOSYSTEM_REVIEW_BLOCKER, false);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_DOWNLOAD,
            "Acquire the selected package through its registered source.",
            UMI_ECOSYSTEM_REVIEW_INFORMATION, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    evidence_severity = UMI_ECOSYSTEM_REVIEW_INFORMATION;
    evidence_approval = false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->evidence == UMI_ECOSYSTEM_EVIDENCE_REJECTED) {
        evidence_severity = UMI_ECOSYSTEM_REVIEW_BLOCKER;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (record->evidence != UMI_ECOSYSTEM_EVIDENCE_VERIFIED) {
        evidence_severity = UMI_ECOSYSTEM_REVIEW_WARNING;
        evidence_approval = true;
    }
    status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_VERIFY,
        "Verify publisher, signature, checksum, SBOM, licence and source evidence.",
        evidence_severity, evidence_approval);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->compatibility == UMI_ECOSYSTEM_INCOMPATIBLE ||
        record->compatibility == UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN) {
        status = add_install_action(out_review,
            UMI_ECOSYSTEM_ACTION_RESOLVE_DEPENDENCIES,
            "Framework ABI, platform or dependency compatibility is unresolved.",
            UMI_ECOSYSTEM_REVIEW_BLOCKER, false);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = add_install_action(out_review,
            UMI_ECOSYSTEM_ACTION_RESOLVE_DEPENDENCIES,
            "Resolve dependencies with the canonical Framework package engines.",
            UMI_ECOSYSTEM_REVIEW_INFORMATION, false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->permission_count > 0U) {
        status = add_install_action(out_review,
            UMI_ECOSYSTEM_ACTION_REVIEW_PERMISSIONS,
            "Review requested extension capabilities before activation.",
            UMI_ECOSYSTEM_REVIEW_WARNING, true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_install_action(out_review,
        record->installed ? UMI_ECOSYSTEM_ACTION_UPDATE : UMI_ECOSYSTEM_ACTION_INSTALL,
        record->installed ? "Apply the selected version update transactionally."
                          : "Install the selected package transactionally.",
        UMI_ECOSYSTEM_REVIEW_INFORMATION, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->requires_migration) {
        status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_MIGRATE,
            "Review and approve versioned project or component migrations.",
            UMI_ECOSYSTEM_REVIEW_WARNING, true);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->kind == UMI_ECOSYSTEM_PACKAGE_EXTENSION) {
        status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_ACTIVATE,
            "Activate through the canonical isolated extension host.",
            UMI_ECOSYSTEM_REVIEW_INFORMATION, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->requires_restart) {
        status = add_install_action(out_review, UMI_ECOSYSTEM_ACTION_RESTART,
            "Restart the affected host after the transaction completes.",
            UMI_ECOSYSTEM_REVIEW_INFORMATION, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->installed) {
        status = umi_ecosystem_install_review_add_action(out_review,
            UMI_ECOSYSTEM_ACTION_ROLLBACK, record->package_id,
            "Retain the previous installed version as a rollback point.",
            UMI_ECOSYSTEM_REVIEW_INFORMATION, false, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem runtime prepare updates operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_runtime_prepare_updates(
    const UmiEcosystemRuntime *runtime,
    UmiEcosystemUpdateReview *out_review)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_ecosystem_update_review_init(out_review);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->packages.package_count; ++index) {
        const UmiEcosystemPackageRecord *record = &runtime->packages.packages[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_ecosystem_package_has_update(record)) continue;
        status = umi_ecosystem_update_review_add(out_review, record);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_runtime_snapshot(
    const UmiEcosystemRuntime *runtime,
    UmiEcosystemRuntimeSnapshot *out_snapshot)
{
    UmiEcosystemPackageStatistics statistics;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_package_catalogue_statistics(
        &runtime->packages, &statistics);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->source_count = runtime->sources.source_count;
    out_snapshot->available_source_count =
        umi_ecosystem_source_available_count(&runtime->sources);
    out_snapshot->package_count = statistics.total;
    out_snapshot->installed_package_count = statistics.installed;
    out_snapshot->update_count = statistics.updates;
    out_snapshot->sdk_count = runtime->sdks.entry_count;
    out_snapshot->installed_sdk_count = runtime->sdks.installed_count;
    out_snapshot->template_count = runtime->components.template_count;
    out_snapshot->offline_mode = runtime->sources.offline_mode;
    out_snapshot->revision = runtime->revision + runtime->sources.revision +
        runtime->packages.revision + runtime->sdks.revision +
        runtime->components.revision;
    return UMI_STATUS_OK;
}
