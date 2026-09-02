/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/hierarchy_navigation_session.c
 *
 * PURPOSE:
 *   Implement lazy, provider-backed call and type hierarchy sessions.  The
 *   Framework normalises parent/depth information, rejects duplicate node
 *   identities, marks cycles, limits expansion, and retains provider reports so
 *   the UI never has to understand transport- or language-specific behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/hierarchy_navigation_session.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorHierarchyNavigationSession {
    UmiEditorNavigationProviderRegistry *registry;
    UmiEditorNavigationHierarchyRequest request;
    UmiEditorHierarchyNavigationEntry *entries;
    size_t count;
    size_t capacity;
    size_t visible_count;
    size_t selected_index;
    UmiEditorNavigationProviderReport *reports;
    size_t report_count;
    size_t report_capacity;
    uint64_t sequence;
    uint64_t revision;
    UmiEditorHierarchyNavigationState state;
    int has_request;
    int cancel_requested;
    int truncated;
    char filter[UMI_EDITOR_HIERARCHY_NAVIGATION_FILTER_CAPACITY];
};

typedef struct HierarchySinkContext {
    UmiEditorHierarchyNavigationSession *session;
    const UmiEditorNavigationProviderRegistration *provider;
} HierarchySinkContext;

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/*
 * Provide the internal cancelled operation used by this module and its client
 * applications.
 */
static int internal_cancelled(void *user_data)
{
    const UmiEditorHierarchyNavigationSession *session =
        (const UmiEditorHierarchyNavigationSession *)user_data;
    return session != NULL && session->cancel_requested;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the valid location operation used by this module and its client applications. */
static int valid_location(const UmiEditorSourceLocation *location)
{
    return location != NULL &&
           umi_editor_source_location_validate(location) == UMI_STATUS_OK &&
           terminated(location->uri, sizeof(location->uri)) &&
           terminated(location->label, sizeof(location->label)) &&
           terminated(location->symbol_id, sizeof(location->symbol_id)) &&
           terminated(location->preview, sizeof(location->preview));
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t start;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    /* Apply this branch only when its contract condition is satisfied. */
    if (pattern_length > text_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + pattern_length <= text_length; ++start) {
        size_t offset;
        int match = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < pattern_length; ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower((unsigned char)text[start + offset]) !=
                tolower((unsigned char)pattern[offset])) {
                match = 0;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match) return 1;
    }
    return 0;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiEditorHierarchyNavigationSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorHierarchyNavigationEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES) {
        session->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->capacity > 0U ? session->capacity : 64U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorHierarchyNavigationEntry *)realloc(
        session->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->entries = replacement;
    session->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve reports operation used by this module and its client applications. */
static UmiStatus reserve_reports(UmiEditorHierarchyNavigationSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationProviderReport *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->report_capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES) {
        session->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->report_capacity > 0U ? session->report_capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorNavigationProviderReport *)realloc(
        session->reports, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->reports = replacement;
    session->report_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find node operation used by this module and its client applications. */
static size_t find_node(const UmiEditorHierarchyNavigationSession *session,
                        const char *provider_id,
                        const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        const UmiEditorNavigationHierarchyNode *node =
            &session->entries[index].node;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(node->node_id, node_id) == 0 &&
            (provider_id == NULL || provider_id[0] == '\0' ||
             strcmp(node->provider_id, provider_id) == 0)) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the parent index operation used by this module and its client applications. */
static size_t parent_index(const UmiEditorHierarchyNavigationSession *session,
                           const UmiEditorNavigationHierarchyNode *node)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (node->parent_node_id[0] == '\0') return SIZE_MAX;
    return find_node(session, node->provider_id, node->parent_node_id);
}

/* Provide the would cycle operation used by this module and its client applications. */
static int would_cycle(const UmiEditorHierarchyNavigationSession *session,
                       const UmiEditorNavigationHierarchyNode *node)
{
    size_t parent = parent_index(session, node);
    size_t guard = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (parent != SIZE_MAX && guard < session->count) {
        const UmiEditorNavigationHierarchyNode *ancestor =
            &session->entries[parent].node;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node->symbol_id[0] != '\0' &&
            strcmp(ancestor->symbol_id, node->symbol_id) == 0) {
            return 1;
        }
        parent = parent_index(session, ancestor);
        ++guard;
    }
    return parent != SIZE_MAX;
}

/* Provide the accept node operation used by this module and its client applications. */
static UmiStatus accept_node(HierarchySinkContext *context,
                             const UmiEditorNavigationHierarchyNode *source)
{
    UmiEditorHierarchyNavigationSession *session;
    UmiEditorHierarchyNavigationEntry entry;
    size_t existing;
    size_t parent;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || source == NULL || context->session == NULL ||
        context->provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session = context->session;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->cancel_requested) return UMI_STATUS_CANCELLED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (source->struct_size != (uint32_t)sizeof(*source) ||
        source->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        !terminated(source->node_id, sizeof(source->node_id)) ||
        !terminated(source->parent_node_id,
                    sizeof(source->parent_node_id)) ||
        !terminated(source->provider_id, sizeof(source->provider_id)) ||
        !terminated(source->symbol_id, sizeof(source->symbol_id)) ||
        !terminated(source->label, sizeof(source->label)) ||
        !terminated(source->detail, sizeof(source->detail)) ||
        source->node_id[0] == '\0' || source->symbol_id[0] == '\0' ||
        source->label[0] == '\0' || !valid_location(&source->location)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.struct_size = (uint32_t)sizeof(entry);
    entry.api_version = UMI_EDITOR_HIERARCHY_NAVIGATION_SESSION_API_VERSION;
    entry.node = *source;
    entry.node.has_children = source->has_children != 0;
    entry.node.expanded = source->expanded != 0;
    entry.node.cycle = source->cycle != 0;
    status = copy_text(entry.node.provider_id,
                       sizeof(entry.node.provider_id),
                       context->provider->descriptor.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    existing = find_node(session, entry.node.provider_id, entry.node.node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (existing != SIZE_MAX) {
        int expanded = session->entries[existing].node.expanded;
        session->entries[existing].node = entry.node;
        session->entries[existing].node.expanded = expanded || entry.node.expanded;
        return UMI_STATUS_OK;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count >= session->request.maximum_nodes ||
        session->count >= UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES) {
        session->truncated = 1;
        return UMI_STATUS_OK;
    }
    parent = parent_index(session, &entry.node);
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent != SIZE_MAX) {
        entry.node.depth = session->entries[parent].node.depth + 1U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (entry.node.parent_node_id[0] == '\0') {
        entry.node.depth = 0U;
        entry.node.expanded = 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (entry.node.depth > session->request.maximum_depth) {
        session->truncated = 1;
        return UMI_STATUS_OK;
    }
    entry.node.cycle = entry.node.cycle || would_cycle(session, &entry.node);
    status = reserve_entries(session, session->count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->entries[session->count++] = entry;
    return UMI_STATUS_OK;
}

/* Provide the hierarchy sink operation used by this module and its client applications. */
static UmiStatus hierarchy_sink(const UmiEditorNavigationHierarchyNode *node,
                                void *user_data)
{
    return accept_node((HierarchySinkContext *)user_data, node);
}

/* Provide the compare entries operation used by this module and its client applications. */
static int compare_entries(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorHierarchyNavigationEntry *left =
        (const UmiEditorHierarchyNavigationEntry *)left_pointer;
    const UmiEditorHierarchyNavigationEntry *right =
        (const UmiEditorHierarchyNavigationEntry *)right_pointer;
    int order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->node.depth < right->node.depth) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->node.depth > right->node.depth) return 1;
    order = strcmp(left->node.parent_node_id, right->node.parent_node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    order = strcmp(left->node.label, right->node.label);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    return strcmp(left->node.node_id, right->node.node_id);
}

/*
 * Provide the sort entries preserving selection operation used by this module and its
 * client applications.
 */
static void sort_entries_preserving_selection(
    UmiEditorHierarchyNavigationSession *session)
{
    char provider_id[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY] = {0};
    char node_id[UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY] = {0};
    int had_selection = session->selected_index != SIZE_MAX &&
                        session->selected_index < session->count;

    /* Apply this branch only when its contract condition is satisfied. */
    if (had_selection) {
        const UmiEditorNavigationHierarchyNode *selected =
            &session->entries[session->selected_index].node;
        (void)copy_text(provider_id, sizeof(provider_id),
                        selected->provider_id);
        (void)copy_text(node_id, sizeof(node_id), selected->node_id);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count > 1U) {
        qsort(session->entries, session->count, sizeof(*session->entries),
              compare_entries);
    }
    session->selected_index = had_selection
        ? find_node(session, provider_id, node_id)
        : SIZE_MAX;
}

/*
 * Provide the ancestors expanded operation used by this module and its client
 * applications.
 */
static int ancestors_expanded(
    const UmiEditorHierarchyNavigationSession *session,
    size_t index)
{
    size_t parent = parent_index(session, &session->entries[index].node);
    size_t guard = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (parent != SIZE_MAX && guard < session->count) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!session->entries[parent].node.expanded) return 0;
        parent = parent_index(session, &session->entries[parent].node);
        ++guard;
    }
    return parent == SIZE_MAX;
}

/* Provide the entry matches operation used by this module and its client applications. */
static int entry_matches(const UmiEditorHierarchyNavigationSession *session,
                         const UmiEditorHierarchyNavigationEntry *entry)
{
    return contains_case_insensitive(entry->node.label, session->filter) ||
           contains_case_insensitive(entry->node.detail, session->filter) ||
           contains_case_insensitive(entry->node.symbol_id, session->filter) ||
           contains_case_insensitive(entry->node.location.uri, session->filter);
}

/*
 * Provide the mark ancestors visible operation used by this module and its client
 * applications.
 */
static void mark_ancestors_visible(UmiEditorHierarchyNavigationSession *session,
                                   size_t index)
{
    size_t parent = parent_index(session, &session->entries[index].node);
    size_t guard = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (parent != SIZE_MAX && guard < session->count) {
        session->entries[parent].visible = 1;
        parent = parent_index(session, &session->entries[parent].node);
        ++guard;
    }
}

/*
 * Provide the recompute visibility operation used by this module and its client
 * applications.
 */
static void recompute_visibility(UmiEditorHierarchyNavigationSession *session)
{
    size_t index;
    size_t first_visible = SIZE_MAX;

    session->visible_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        session->entries[index].visible =
            session->filter[0] == '\0' && ancestors_expanded(session, index);
        session->entries[index].selected = 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->filter[0] != '\0') {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < session->count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (entry_matches(session, &session->entries[index])) {
                session->entries[index].visible = 1;
                mark_ancestors_visible(session, index);
            }
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].visible) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (first_visible == SIZE_MAX) first_visible = index;
            ++session->visible_count;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->selected_index >= session->count ||
        (session->selected_index != SIZE_MAX &&
         !session->entries[session->selected_index].visible)) {
        session->selected_index = first_visible;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index != SIZE_MAX) {
        session->entries[session->selected_index].selected = 1;
    }
}

/* Provide the initialise report operation used by this module and its client applications. */
static void initialise_report(UmiEditorNavigationProviderReport *report,
                              const char *provider_id,
                              uint64_t sequence)
{
    (void)memset(report, 0, sizeof(*report));
    report->struct_size = (uint32_t)sizeof(*report);
    report->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    report->status = UMI_STATUS_OK;
    report->sequence = sequence;
    (void)copy_text(report->provider_id, sizeof(report->provider_id), provider_id);
}

/* Provide the dispatch provider operation used by this module and its client applications. */
static UmiStatus dispatch_provider(
    UmiEditorHierarchyNavigationSession *session,
    const UmiEditorNavigationProviderRegistration *registration,
    const UmiEditorNavigationHierarchyRequest *request)
{
    UmiEditorNavigationProviderReport report;
    UmiEditorNavigationCancellation cancellation;
    HierarchySinkContext context;
    size_t before_count;
    UmiStatus status;
    UmiStatus reserve_status;

    reserve_status = reserve_reports(session, session->report_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (reserve_status != UMI_STATUS_OK) return reserve_status;
    initialise_report(&report, registration->descriptor.id, ++session->sequence);
    cancellation.struct_size = (uint32_t)sizeof(cancellation);
    cancellation.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    cancellation.is_cancelled = internal_cancelled;
    cancellation.user_data = session;
    context.session = session;
    context.provider = registration;
    before_count = session->count;
    status = registration->functions.hierarchy(
        registration->instance, request, &cancellation, hierarchy_sink, &context,
        &report);
    /* Apply this branch only when its contract condition is satisfied. */
    if (report.struct_size != (uint32_t)sizeof(report) ||
        report.api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        !terminated(report.message, sizeof(report.message))) {
        initialise_report(&report, registration->descriptor.id,
                          session->sequence);
    }
    (void)copy_text(report.provider_id, sizeof(report.provider_id),
                    registration->descriptor.id);
    report.status = status;
    report.emitted_count = session->count - before_count;
    report.cancelled = session->cancel_requested || status == UMI_STATUS_CANCELLED;
    report.truncated = session->truncated;
    session->reports[session->report_count++] = report;
    return status;
}

/*
 * Initialise editor hierarchy navigation session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_hierarchy_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorHierarchyNavigationSession **out_session)
{
    UmiEditorHierarchyNavigationSession *session;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorHierarchyNavigationSession *)calloc(1U,
                                                             sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->registry = registry;
    session->selected_index = SIZE_MAX;
    session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_CLOSED;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor hierarchy navigation session so the same storage
 * can be reused safely.
 */
void umi_editor_hierarchy_navigation_session_destroy(
    UmiEditorHierarchyNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    free(session->entries);
    free(session->reports);
    session->entries = NULL;
    session->reports = NULL;
    free(session);
}

/*
 * Provide the editor hierarchy navigation session open operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_open(
    UmiEditorHierarchyNavigationSession *session,
    const UmiEditorNavigationHierarchyRequest *request)
{
    UmiEditorNavigationProviderRegistration registration;
    size_t index;
    size_t matched = 0U;
    UmiStatus first_failure = UMI_STATUS_NOT_FOUND;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U || request->root_symbol_id[0] == '\0' ||
        request->kind < UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING ||
        request->kind > UMI_EDITOR_NAVIGATION_HIERARCHY_TYPE_SUBTYPES ||
        !terminated(request->root_symbol_id,
                    sizeof(request->root_symbol_id)) ||
        !terminated(request->expand_node_id,
                    sizeof(request->expand_node_id)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        request->maximum_depth == 0U ||
        request->maximum_depth >
            UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES ||
        request->maximum_nodes == 0U ||
        request->maximum_nodes > UMI_EDITOR_NAVIGATION_MAXIMUM_HIERARCHY_NODES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->request = *request;
    session->count = 0U;
    session->visible_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->cancel_requested = 0;
    session->truncated = 0;
    session->has_request = 1;
    session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_LOADING;
    session->revision = next_revision(session->revision);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(session->registry);
         ++index) {
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_navigation_provider_registry_at(session->registry, index,
                                                       &registration) !=
                UMI_STATUS_OK ||
            !umi_editor_navigation_provider_supports_hierarchy(&registration,
                                                               request)) {
            continue;
        }
        ++matched;
        status = dispatch_provider(session, &registration, request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND &&
            status != UMI_STATUS_CANCELLED &&
            first_failure == UMI_STATUS_NOT_FOUND) {
            first_failure = status;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CANCELLED || session->cancel_requested) break;
    }
    sort_entries_preserving_selection(session);
    recompute_visibility(session);
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->cancel_requested) {
        session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_CANCELLED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_CANCELLED;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched == 0U) {
        session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count == 0U && first_failure != UMI_STATUS_NOT_FOUND) {
        session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return first_failure;
    }
    session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_refresh(
    UmiEditorHierarchyNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_hierarchy_navigation_session_open(session,
                                                        &session->request);
}

/*
 * Provide the editor hierarchy navigation session cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_cancel(
    UmiEditorHierarchyNavigationSession *session)
{
    UmiEditorNavigationProviderRegistration registration;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_HIERARCHY_NAVIGATION_LOADING) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->cancel_requested = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(session->registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_navigation_provider_registry_at(session->registry, index,
                                                       &registration) ==
                UMI_STATUS_OK &&
            umi_editor_navigation_provider_supports_hierarchy(
                &registration, &session->request) &&
            registration.functions.cancel != NULL) {
            (void)registration.functions.cancel(registration.instance,
                                                session->request.request_id);
        }
    }
    session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session close operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_close(
    UmiEditorHierarchyNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session->count = 0U;
    session->visible_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->has_request = 0;
    session->cancel_requested = 0;
    session->truncated = 0;
    session->filter[0] = '\0';
    session->state = UMI_EDITOR_HIERARCHY_NAVIGATION_CLOSED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session expand operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_expand(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id)
{
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorNavigationHierarchyRequest request;
    size_t index;
    size_t node_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    node_index = find_node(session, NULL, node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (node_index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!session->entries[node_index].node.has_children ||
        session->entries[node_index].node.cycle) {
        session->entries[node_index].node.expanded = 1;
        recompute_visibility(session);
        session->revision = next_revision(session->revision);
        return UMI_STATUS_OK;
    }
    request = session->request;
    status = copy_text(request.expand_node_id, sizeof(request.expand_node_id),
                       session->entries[node_index].node.node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(request.root_symbol_id, sizeof(request.root_symbol_id),
                           session->entries[node_index].node.symbol_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(session->registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_navigation_provider_registry_at(session->registry, index,
                                                       &registration) ==
                UMI_STATUS_OK &&
            strcmp(registration.descriptor.id,
                   session->entries[node_index].node.provider_id) == 0 &&
            umi_editor_navigation_provider_supports_hierarchy(&registration,
                                                               &request)) {
            status = dispatch_provider(session, &registration, &request);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
                return status;
            }
            break;
        }
    }
    node_index = find_node(session, NULL, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (node_index != SIZE_MAX) session->entries[node_index].node.expanded = 1;
    sort_entries_preserving_selection(session);
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session collapse operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_collapse(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(session, NULL, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    session->entries[index].node.expanded = 0;
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session expand all operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_expand_all(
    UmiEditorHierarchyNavigationSession *session)
{
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < session->count) {
        char node_id[UMI_EDITOR_NAVIGATION_HIERARCHY_NODE_ID_CAPACITY];
        int expandable = session->entries[index].node.has_children &&
                         !session->entries[index].node.expanded &&
                         !session->entries[index].node.cycle &&
                         session->entries[index].node.depth <
                             session->request.maximum_depth;
        /* Apply this branch only when its contract condition is satisfied. */
        if (expandable) {
            UmiStatus status = copy_text(
                node_id, sizeof(node_id), session->entries[index].node.node_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            status = umi_editor_hierarchy_navigation_session_expand(session,
                                                                    node_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
                return status;
            }
            index = 0U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            ++index;
        }
    }
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session collapse all operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_collapse_all(
    UmiEditorHierarchyNavigationSession *session)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        session->entries[index].node.expanded = 0;
    }
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session set filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_set_filter(
    UmiEditorHierarchyNavigationSession *session,
    const char *filter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(session->filter, sizeof(session->filter), filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session select operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select(
    UmiEditorHierarchyNavigationSession *session,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(session, NULL, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX || !session->entries[index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index != SIZE_MAX &&
        session->selected_index < session->count) {
        session->entries[session->selected_index].selected = 0;
    }
    session->selected_index = index;
    session->entries[index].selected = 1;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/* Provide the select relative operation used by this module and its client applications. */
static UmiStatus select_relative(UmiEditorHierarchyNavigationSession *session,
                                 int direction,
                                 int wrap)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->visible_count == 0U) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < session->count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_hierarchy_navigation_session_select(
                    session, session->entries[index].node.node_id);
            }
        }
    } else /* Apply this branch only when its contract condition is satisfied. */ if (direction > 0) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = session->selected_index + 1U; index < session->count;
             ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_hierarchy_navigation_session_select(
                    session, session->entries[index].node.node_id);
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (wrap) {
            /* Visit each bounded item once so every record receives the same rule. */
            for (index = 0U; index < session->selected_index; ++index) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (session->entries[index].visible) {
                    return umi_editor_hierarchy_navigation_session_select(
                        session, session->entries[index].node.node_id);
                }
            }
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        index = session->selected_index;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (index > 0U) {
            --index;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_hierarchy_navigation_session_select(
                    session, session->entries[index].node.node_id);
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (wrap) {
            index = session->count;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (index > session->selected_index + 1U) {
                --index;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (session->entries[index].visible) {
                    return umi_editor_hierarchy_navigation_session_select(
                        session, session->entries[index].node.node_id);
                }
            }
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor hierarchy navigation session select next operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select_next(
    UmiEditorHierarchyNavigationSession *session,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, 1, wrap != 0);
}

/*
 * Provide the editor hierarchy navigation session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_select_previous(
    UmiEditorHierarchyNavigationSession *session,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, -1, wrap != 0);
}

/*
 * Find editor hierarchy navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorHierarchyNavigationEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = session->entries[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor hierarchy navigation session visible while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_visible_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t visible_position,
    UmiEditorHierarchyNavigationEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].visible) {
            /* Apply this operation only while the related capability or state is available. */
            if (current == visible_position) {
                *out_entry = session->entries[index];
                return UMI_STATUS_OK;
            }
            ++current;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find editor hierarchy navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_selected(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX ||
        session->selected_index >= session->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = session->entries[session->selected_index];
    return UMI_STATUS_OK;
}

/*
 * Find editor hierarchy navigation session provider report while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_hierarchy_navigation_session_provider_report_at(
    const UmiEditorHierarchyNavigationSession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->report_count) return UMI_STATUS_NOT_FOUND;
    *out_report = session->reports[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor hierarchy navigation session snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_hierarchy_navigation_session_snapshot(
    const UmiEditorHierarchyNavigationSession *session,
    UmiEditorHierarchyNavigationSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_HIERARCHY_NAVIGATION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->hierarchy_kind = session->has_request
        ? session->request.kind
        : UMI_EDITOR_NAVIGATION_HIERARCHY_CALL_INCOMING;
    out_snapshot->node_count = session->count;
    out_snapshot->visible_node_count = session->visible_count;
    out_snapshot->selected_index = session->selected_index;
    out_snapshot->provider_count = session->report_count;
    out_snapshot->request_id = session->has_request
        ? session->request.request_id
        : 0U;
    out_snapshot->revision = session->revision;
    out_snapshot->has_selection = session->selected_index != SIZE_MAX;
    out_snapshot->truncated = session->truncated;
    out_snapshot->cancelled = session->cancel_requested;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].node.expanded) {
            ++out_snapshot->expanded_node_count;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].node.cycle) ++out_snapshot->cycle_count;
    }
    (void)copy_text(out_snapshot->filter, sizeof(out_snapshot->filter),
                    session->filter);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor hierarchy navigation session without
 * changing their state.
 */
size_t umi_editor_hierarchy_navigation_session_count(
    const UmiEditorHierarchyNavigationSession *session)
{
    return session != NULL ? session->count : 0U;
}

/*
 * Return the number of records represented by editor hierarchy navigation session visible
 * without changing their state.
 */
size_t umi_editor_hierarchy_navigation_session_visible_count(
    const UmiEditorHierarchyNavigationSession *session)
{
    return session != NULL ? session->visible_count : 0U;
}

/*
 * Provide the editor hierarchy navigation session revision operation used by this module
 * and its client applications.
 */
uint64_t umi_editor_hierarchy_navigation_session_revision(
    const UmiEditorHierarchyNavigationSession *session)
{
    return session != NULL ? session->revision : 0U;
}
