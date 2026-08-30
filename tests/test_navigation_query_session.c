/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_navigation_query_session.c
 *
 * PURPOSE:
 *   Implement the test navigation query session behavior for
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
 * Umicom Framework multi-provider navigation query tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#define COPY_TEXT(destination, source)                                      \
    do {                                                                     \
        const char *umi_copy_source_local = (source);                        \
        size_t umi_copy_length_local = strlen(umi_copy_source_local);        \
        assert(umi_copy_length_local < sizeof(destination));                 \
        (void)memcpy((destination), umi_copy_source_local,                   \
                     umi_copy_length_local + 1U);                            \
    } while (0)

#include "umicom/editor/navigation_query_session.h"

typedef struct FakeQueryProvider {
    int provider_number;
} FakeQueryProvider;

static UmiStatus emit_result(UmiEditorNavigationResultSink sink,
                             void *sink_user_data,
                             const char *uri,
                             uint64_t line,
                             uint64_t revision,
                             int32_t rank,
                             int primary)
{
    UmiEditorNavigationResult result;
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    assert(umi_editor_source_location_initialize(&result.location, uri, line,
                                                 0U) == UMI_STATUS_OK);
    result.location.kind = UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
    result.location.document_revision = revision;
    result.location.byte_offset = line * 10U;
    result.location.end_byte_offset = result.location.byte_offset + 4U;
    COPY_TEXT(result.location.symbol_id, "symbol.target");
    COPY_TEXT(result.location.label, "target");
    result.rank = rank;
    result.primary = primary;
    return sink(&result, sink_user_data);
}

static UmiStatus fake_query(
    void *instance,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationResultSink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    FakeQueryProvider *provider = (FakeQueryProvider *)instance;
    (void)request;
    (void)out_report;
    if (cancellation->is_cancelled(cancellation->user_data)) {
        return UMI_STATUS_CANCELLED;
    }
    assert(emit_result(sink, sink_user_data, "file:///workspace/target.c",
                       10U, 7U,
                       provider->provider_number == 2 ? 50 : 10,
                       provider->provider_number == 2) == UMI_STATUS_OK);
    if (provider->provider_number == 1) {
        /* A revision from another document is independent and must remain. */
        assert(emit_result(sink, sink_user_data,
                           "file:///workspace/other.c", 20U, 6U, 100, 0) ==
               UMI_STATUS_OK);
        /* A mismatched revision in the originating document is stale. */
        assert(emit_result(sink, sink_user_data,
                           "file:///workspace/main.c", 30U, 6U, 200, 0) ==
               UMI_STATUS_OK);
    }
    return UMI_STATUS_OK;
}

static UmiEditorNavigationProviderRegistration registration_for(
    FakeQueryProvider *provider,
    const char *id,
    int32_t priority)
{
    UmiEditorNavigationProviderRegistration registration;
    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, id, id, "c",
               UMI_EDITOR_NAVIGATION_CAPABILITY_DEFINITION, priority) ==
           UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.query = fake_query;
    registration.instance = provider;
    return registration;
}

int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationQuerySession *session = NULL;
    FakeQueryProvider low = {1};
    FakeQueryProvider high = {2};
    UmiEditorNavigationProviderRegistration low_registration =
        registration_for(&low, "provider.low", 1);
    UmiEditorNavigationProviderRegistration high_registration =
        registration_for(&high, "provider.high", 5);
    UmiEditorSourceLocation origin;
    UmiEditorNavigationRequest request;
    UmiEditorNavigationQueryOptions options;
    UmiEditorNavigationQuerySessionSnapshot snapshot;
    UmiEditorNavigationResult selected;

    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(
               registry, &low_registration) == UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(
               registry, &high_registration) == UMI_STATUS_OK);
    assert(umi_editor_navigation_query_session_create(registry, &session) ==
           UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(
               &origin, "file:///workspace/main.c", 4U, 1U) == UMI_STATUS_OK);
    origin.document_revision = 7U;
    assert(umi_editor_navigation_request_initialize(
               &request, 77U, UMI_EDITOR_NAVIGATION_QUERY_DEFINITION,
               &origin, "c") == UMI_STATUS_OK);
    COPY_TEXT(request.symbol_id, "symbol.target");
    assert(umi_editor_navigation_query_options_initialize(&options) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_query_session_execute(
               session, &request, &options) == UMI_STATUS_OK);
    assert(umi_editor_navigation_query_session_snapshot(session, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.result_count == 2U);
    assert(snapshot.duplicate_result_count == 1U);
    assert(snapshot.stale_result_count == 1U);
    assert(snapshot.provider_count == 2U);
    assert(umi_editor_navigation_query_session_refresh(session) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_query_session_count(session) == 2U);
    assert(umi_editor_navigation_query_session_selected(session, &selected) ==
           UMI_STATUS_OK);
    assert(strcmp(selected.provider_id, "provider.high") == 0);
    assert(selected.primary);
    assert(umi_editor_navigation_query_session_select_next(session) ==
           UMI_STATUS_OK);
    umi_editor_navigation_query_session_destroy(session);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
