/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_runtime.c
 * PURPOSE: Verify end-to-end catalogue, query, install and update coordination.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/runtime.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEcosystemRuntime *runtime = NULL;
    UmiEcosystemSource source;
    UmiEcosystemPackageRecord package;
    UmiEcosystemPackageQuery query;
    UmiEcosystemPackageQueryResult result;
    UmiEcosystemInstallReview install;
    UmiEcosystemUpdateReview updates;
    UmiEcosystemRuntimeSnapshot snapshot;
    assert(umi_ecosystem_runtime_create(&runtime) == UMI_STATUS_OK);
    umi_ecosystem_source_init(&source, "official", "Official",
        UMI_ECOSYSTEM_SOURCE_REMOTE, "https://packages.example.test");
    source.trusted = true;
    assert(umi_ecosystem_source_catalogue_upsert(
        umi_ecosystem_runtime_sources(runtime), &source) == UMI_STATUS_OK);
    umi_ecosystem_package_record_init(&package, "org.umicom.extension",
        "Umicom Extension", UMI_ECOSYSTEM_PACKAGE_EXTENSION,
        "official", "2.0.0");
    assert(umi_ecosystem_copy_text(package.installed_version,
        sizeof(package.installed_version), "1.0.0") == UMI_STATUS_OK);
    package.installed = true;
    package.state = UMI_ECOSYSTEM_UPDATE_AVAILABLE;
    package.compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    package.evidence = UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
    package.permission_count = 1U;
    package.requires_restart = true;
    assert(umi_ecosystem_package_catalogue_upsert(
        umi_ecosystem_runtime_packages(runtime), &package) == UMI_STATUS_OK);
    umi_ecosystem_package_query_init(&query);
    query.available_sources_only = true;
    assert(umi_ecosystem_runtime_query(runtime, &query, &result) == UMI_STATUS_OK);
    assert(result.result_count == 1U);
    assert(umi_ecosystem_runtime_prepare_install(runtime, package.package_id,
        &install) == UMI_STATUS_OK);
    assert(install.pending_approval_count == 1U && install.requires_restart);
    assert(umi_ecosystem_runtime_prepare_updates(runtime, &updates) ==
        UMI_STATUS_OK);
    assert(updates.candidate_count == 1U);
    assert(umi_ecosystem_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.package_count == 1U && snapshot.update_count == 1U);
    assert(umi_ecosystem_runtime_set_offline(runtime, true) == UMI_STATUS_OK);
    assert(umi_ecosystem_runtime_snapshot(runtime, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.offline_mode && snapshot.available_source_count == 0U);
    umi_ecosystem_runtime_destroy(runtime);
    return 0;
}
