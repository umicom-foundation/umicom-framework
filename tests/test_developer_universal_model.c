/* Umicom Framework universal development model regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include <assert.h>
#include <string.h>

#include "umicom/developer/universal_model.h"

int main(void)
{
    UmiDeveloperUniversalModel *model = NULL;
    UmiDeveloperUniversalModelSnapshot snapshot;
    UmiDeveloperUniversalResolutionSnapshot resolution;
    UmiDeveloperUniversalProjectAudit audit;
    UmiProjectLanguageEntrySnapshot entry;

    assert(umi_developer_universal_model_create(&model) == UMI_STATUS_OK);
    assert(umi_developer_universal_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.languages.profile_count >= 20U);
    assert(snapshot.toolchains.binding_count >= 20U);

    assert(umi_developer_universal_model_register_project(
               model, "umicom-studio", "c") == UMI_STATUS_OK);
    umi_project_language_entry_init(&entry);
    strcpy(entry.language_id, "cpp");
    strcpy(entry.source_roots, "adapters/qt;adapters/wt");
    strcpy(entry.standard, "C++23");
    entry.compile_order = 1U;
    entry.interop_policy = UMI_PROJECT_LANGUAGE_INTEROP_C_ABI;
    assert(umi_developer_universal_model_configure_language(
               model, "umicom-studio", &entry) == UMI_STATUS_OK);

    assert(umi_developer_universal_model_resolve(
               model, "umicom-studio", "src/main.c", "windows", "x86_64",
               UMI_LANGUAGE_CAPABILITY_BUILD, &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.language.id, "c") == 0);
    assert(strcmp(resolution.toolchain.id, "c.msvc") == 0);
    assert(resolution.has_project_language != 0);
    assert(resolution.has_toolchain != 0);

    assert(umi_developer_universal_model_resolve(
               model, "umicom-studio", "tools/generate.py", "windows", "x86_64",
               UMI_LANGUAGE_CAPABILITY_RUN, &resolution) == UMI_STATUS_UNAVAILABLE);
    umi_project_language_entry_init(&entry);
    strcpy(entry.language_id, "python");
    strcpy(entry.source_roots, "tools;scripts");
    strcpy(entry.standard, "Python 3");
    entry.compile_order = 2U;
    entry.interop_policy = UMI_PROJECT_LANGUAGE_INTEROP_FOREIGN_RUNTIME;
    assert(umi_developer_universal_model_configure_language(
               model, "umicom-studio", &entry) == UMI_STATUS_OK);
    assert(umi_developer_universal_model_resolve(
               model, "umicom-studio", "tools/generate.py", "windows", "x86_64",
               UMI_LANGUAGE_CAPABILITY_RUN, &resolution) == UMI_STATUS_OK);
    assert(strcmp(resolution.toolchain.id, "python.cpython") == 0);

    assert(umi_developer_universal_model_audit_project(
               model, "umicom-studio", "windows", "x86_64", &audit) ==
           UMI_STATUS_OK);
    assert(audit.enabled_language_count == 3U);
    assert(audit.resolved_toolchain_count == 3U);
    assert(audit.primary_language_ready != 0);
    assert(audit.unresolved_count == 0U);
    umi_developer_universal_model_destroy(model);
    return 0;
}
