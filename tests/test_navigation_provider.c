/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_navigation_provider.c
 *
 * PURPOSE:
 *   Implement the test navigation provider behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework navigation provider registry tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/navigation_provider.h"

/*
 * Exercise unused query and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus unused_query(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationResultSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    (void)instance;
    (void)request;
    (void)cancellation;
    (void)sink;
    (void)sink_user_data;
    (void)out_report;
    return UMI_STATUS_OK;
}

/*
 * Exercise make registration and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiEditorNavigationProviderRegistration make_registration(
    const char *id,
    const char *languages,
    int32_t priority)
{
    UmiEditorNavigationProviderRegistration registration;
    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, id, id, languages,
               UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION, priority) ==
           UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.query = unused_query;
    return registration;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationProviderRegistration first =
        make_registration("provider.first", "c,cpp", 10);
    UmiEditorNavigationProviderRegistration second =
        make_registration("provider.second", "*", 20);
    UmiEditorNavigationProviderRegistration found;
    UmiEditorNavigationProviderRegistrySnapshot snapshot;
    UmiEditorSourceLocation origin;
    UmiEditorNavigationRequest request;

    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry, &second) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_count(registry) == 2U);
    assert(umi_editor_navigation_provider_registry_at(registry, 0U, &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.descriptor.id, "provider.second") == 0);
    assert(umi_editor_navigation_provider_registry_find(
               registry, "provider.first", &found) == UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_language_matches(
               &found.descriptor, "c"));
    assert(umi_editor_navigation_provider_language_matches(
               &found.descriptor, "CPP"));
    assert(!umi_editor_navigation_provider_language_matches(
               &found.descriptor, "python"));

    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 3U, 2U) == UMI_STATUS_OK);
    origin.document_revision = 9U;
    assert(umi_editor_navigation_request_initialize(
               &request, 42U, UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &origin, "c") == UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_supports_request(&found, &request));
    assert(umi_editor_navigation_capability_for_query_kind(
               UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) ==
           UMI_EDITOR_NAVIGATION_CAPABILITY_REFERENCES);

    assert(umi_editor_navigation_provider_registry_snapshot(registry,
                                                            &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.provider_count == 2U);
    assert(snapshot.enabled_count == 2U);
    assert(snapshot.revision > 1U);
    assert(umi_editor_navigation_provider_registry_remove(
               registry, "provider.first") == UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_count(registry) == 1U);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
