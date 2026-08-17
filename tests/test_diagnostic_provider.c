/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_provider.c
 *
 * PURPOSE:
 *   Verify professional diagnostic provider registration, copied finding
 *   batches, metadata, fixes, revisions and deterministic lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

int main(void)
{
    UmiDiagnosticProviderRegistry *registry = NULL;
    UmiDiagnosticProviderBatch *batch = NULL;
    UmiDiagnosticProviderRegistrySnapshot registry_snapshot;
    UmiDiagnosticProviderBatchSnapshot batch_snapshot;
    UmiDiagnosticProviderDescriptor provider = test_provider("compiler.gcc", 50);
    UmiDiagnosticProviderDescriptor found;
    UmiDiagnosticProviderFinding first = test_finding(
        "compiler.gcc", "gcc-1", UMI_DIAGNOSTIC_WARNING, "-Wshadow",
        "declaration shadows an earlier local", "src/main.c", 14U, 10U, 1);
    UmiDiagnosticProviderFinding second = test_finding(
        "compiler.gcc", "gcc-2", UMI_DIAGNOSTIC_ERROR, "E100",
        "undefined symbol", "src/link.c", 42U, 10U, 0);

    assert(umi_diagnostic_provider_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_registry_upsert(registry, &provider) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_provider_registry_find(registry, "compiler.gcc",
                                                 &found) == UMI_STATUS_OK);
    assert(strcmp(found.tool_name, "umi-analyzer") == 0);
    assert(umi_diagnostic_provider_registry_snapshot(registry,
                                                      &registry_snapshot) ==
           UMI_STATUS_OK);
    assert(registry_snapshot.provider_count == 1U);
    assert(registry_snapshot.fix_provider_count == 1U);
    assert(registry_snapshot.sarif_provider_count == 1U);

    assert(umi_diagnostic_provider_batch_create(&batch) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &first) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_upsert(batch, &second) == UMI_STATUS_OK);
    assert(umi_diagnostic_provider_batch_snapshot(batch, &batch_snapshot) ==
           UMI_STATUS_OK);
    assert(batch_snapshot.finding_count == 2U);
    assert(batch_snapshot.fixable_count == 1U);
    assert(batch_snapshot.provider_count == 1U);
    assert(batch_snapshot.by_severity[UMI_DIAGNOSTIC_WARNING] == 1U);
    assert(batch_snapshot.by_severity[UMI_DIAGNOSTIC_ERROR] == 1U);
    assert(first.fingerprint != 0U);
    assert(first.content_fingerprint != 0U);
    assert(first.fingerprint != second.fingerprint);

    umi_diagnostic_provider_batch_destroy(batch);
    umi_diagnostic_provider_registry_destroy(registry);
    return 0;
}
