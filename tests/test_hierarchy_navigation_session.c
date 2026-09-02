/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_hierarchy_navigation_session.c
 *
 * PURPOSE:
 *   Implement the test hierarchy navigation session behavior for
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
 * Umicom Framework lazy call/type hierarchy tests.
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

#include "umicom/editor/hierarchy_navigation_session.h"

/*
 * Exercise emit node and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus emit_node(UmiEditorNavigationHierarchySink sink,
                           void *user_data,
                           const char *node_id,
                           const char *parent_id,
                           const char *symbol_id,
                           const char *label,
                           uint64_t line,
                           int has_children)
{
    UmiEditorNavigationHierarchyNode node;
    (void)memset(&node, 0, sizeof(node));
    node.struct_size = (uint32_t)sizeof(node);
    node.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    COPY_TEXT(node.node_id, node_id);
    COPY_TEXT(node.parent_node_id, parent_id);
    COPY_TEXT(node.symbol_id, symbol_id);
    COPY_TEXT(node.label, label);
    COPY_TEXT(node.detail, label);
    assert(umi_editor_source_location_initialize(
               &node.location, "file:///workspace/hierarchy.c", line, 0U) ==
           UMI_STATUS_OK);
    node.location.kind = UMI_EDITOR_SOURCE_LOCATION_SYMBOL;
    node.location.byte_offset = line * 10U;
    node.location.end_byte_offset = node.location.byte_offset + 4U;
    node.has_children = has_children;
    return sink(&node, user_data);
}

/*
 * Exercise fake hierarchy and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus fake_hierarchy(
    void *instance,
    const UmiEditorNavigationHierarchyRequest *request,
    const UmiEditorNavigationCancellation *cancellation,
    UmiEditorNavigationHierarchySink sink,
    void *sink_user_data,
    UmiEditorNavigationProviderReport *out_report)
{
    (void)instance;
    (void)out_report;
    assert(!cancellation->is_cancelled(cancellation->user_data));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (request->expand_node_id[0] == '\0') {
        return emit_node(sink, sink_user_data, "root", "", "symbol.root",
                         "Root Function", 1U, 1);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(request->expand_node_id, "root") == 0) {
        assert(emit_node(sink, sink_user_data, "child", "root",
                         "symbol.child", "Child Function", 5U, 0) ==
               UMI_STATUS_OK);
        assert(emit_node(sink, sink_user_data, "cycle", "child",
                         "symbol.root", "Recursive Root", 9U, 0) ==
               UMI_STATUS_OK);
    }
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorNavigationProviderRegistry *registry = NULL;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorHierarchyNavigationSession *session = NULL;
    UmiEditorNavigationHierarchyRequest request;
    UmiEditorHierarchyNavigationSnapshot snapshot;
    UmiEditorHierarchyNavigationEntry entry;

    (void)memset(&registration, 0, sizeof(registration));
    registration.struct_size = (uint32_t)sizeof(registration);
    registration.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    assert(umi_editor_navigation_provider_descriptor_initialize(
               &registration.descriptor, "hierarchy.provider", "Hierarchy",
               "c", UMI_EDITOR_NAVIGATION_CAPABILITY_CALL_HIERARCHY, 10) ==
           UMI_STATUS_OK);
    registration.functions.struct_size =
        (uint32_t)sizeof(registration.functions);
    registration.functions.api_version =
        UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    registration.functions.hierarchy = fake_hierarchy;
    assert(umi_editor_navigation_provider_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_provider_registry_upsert(registry,
                                                          &registration) ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_create(registry, &session) ==
           UMI_STATUS_OK);
    assert(umi_editor_navigation_hierarchy_request_initialize(
               &request, 123U,
               UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING,
               "symbol.root", "c") == UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_open(session, &request) ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_count(session) == 1U);
    assert(umi_editor_hierarchy_navigation_session_select(session, "root") ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_expand(session, "root") ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_selected(session, &entry) ==
           UMI_STATUS_OK);
    assert(strcmp(entry.node.node_id, "root") == 0);
    assert(umi_editor_hierarchy_navigation_session_snapshot(session,
                                                            &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.node_count == 3U);
    assert(snapshot.cycle_count == 1U);
    assert(umi_editor_hierarchy_navigation_session_select(session, "child") ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_selected(session, &entry) ==
           UMI_STATUS_OK);
    assert(strcmp(entry.node.symbol_id, "symbol.child") == 0);
    assert(umi_editor_hierarchy_navigation_session_set_filter(session,
                                                              "Child") ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_visible_count(session) ==
           2U);
    assert(umi_editor_hierarchy_navigation_session_collapse_all(session) ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_set_filter(session, "") ==
           UMI_STATUS_OK);
    assert(umi_editor_hierarchy_navigation_session_visible_count(session) ==
           1U);
    assert(umi_editor_hierarchy_navigation_session_expand_all(session) ==
           UMI_STATUS_OK);
    request.kind = (UmiEditorNavigationHierarchyKind)99;
    assert(umi_editor_hierarchy_navigation_session_open(session, &request) ==
           UMI_STATUS_INVALID_ARGUMENT);
    umi_editor_hierarchy_navigation_session_destroy(session);
    umi_editor_navigation_provider_registry_destroy(registry);
    return 0;
}
