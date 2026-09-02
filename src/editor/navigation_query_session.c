/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_query_session.c
 *
 * PURPOSE:
 *   Implement deterministic multi-provider navigation queries.  Provider
 *   failures are retained as evidence, result provenance is normalised at the
 *   Framework boundary, duplicate locations are collapsed, revision mismatch
 *   is reported as stale state, and ranking remains independent of the order in
 *   which application frontends render the results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_query_session.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationQuerySession {
    UmiEditorNavigationProviderRegistry *registry;
    UmiEditorNavigationRequest request;
    UmiEditorNavigationQueryOptions options;
    UmiEditorNavigationResult *results;
    size_t result_count;
    size_t result_capacity;
    UmiEditorNavigationProviderReport *reports;
    size_t report_count;
    size_t report_capacity;
    size_t selected_index;
    size_t stale_result_count;
    size_t duplicate_result_count;
    size_t failed_provider_count;
    uint64_t registry_revision;
    uint64_t sequence;
    uint64_t revision;
    UmiEditorNavigationQuerySessionState state;
    int has_request;
    int cancel_requested;
    int truncated;
};

typedef struct QuerySinkContext {
    UmiEditorNavigationQuerySession *session;
    const UmiEditorNavigationProviderRegistration *provider;
} QuerySinkContext;

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
    const UmiEditorNavigationQuerySession *session =
        (const UmiEditorNavigationQuerySession *)user_data;
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

/* Provide the valid result operation used by this module and its client applications. */
static int valid_result(const UmiEditorNavigationResult *result)
{
    return result != NULL &&
           result->struct_size == (uint32_t)sizeof(*result) &&
           result->api_version == UMI_EDITOR_NAVIGATION_RESULT_API_VERSION &&
           terminated(result->provider_id, sizeof(result->provider_id)) &&
           terminated(result->detail, sizeof(result->detail)) &&
           valid_location(&result->location);
}

/* Provide the reserve results operation used by this module and its client applications. */
static UmiStatus reserve_results(UmiEditorNavigationQuerySession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationResult *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->result_capacity) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS) {
        session->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->result_capacity > 0U ? session->result_capacity : 32U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorNavigationResult *)realloc(
        session->results, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->results = replacement;
    session->result_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve reports operation used by this module and its client applications. */
static UmiStatus reserve_reports(UmiEditorNavigationQuerySession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationProviderReport *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->report_capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->report_capacity > 0U ? session->report_capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY;
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

/* Provide the same result operation used by this module and its client applications. */
static int same_result(const UmiEditorNavigationResult *left,
                       const UmiEditorNavigationResult *right)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(left->location.uri, right->location.uri) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->location.byte_offset != right->location.byte_offset ||
        left->location.end_byte_offset != right->location.end_byte_offset ||
        left->location.line != right->location.line ||
        left->location.column != right->location.column) {
        return 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (left->location.symbol_id[0] != '\0' &&
        right->location.symbol_id[0] != '\0' &&
        strcmp(left->location.symbol_id, right->location.symbol_id) != 0) {
        return 0;
    }
    return 1;
}

/* Provide the bounded add score operation used by this module and its client applications. */
static int32_t bounded_add_score(int32_t value, int64_t addition)
{
    int64_t combined = (int64_t)value + addition;
    /* Apply this branch only when its contract condition is satisfied. */
    if (combined > INT32_MAX) return INT32_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (combined < INT32_MIN) return INT32_MIN;
    return (int32_t)combined;
}

/* Provide the result is stale operation used by this module and its client applications. */
static int result_is_stale(const UmiEditorNavigationQuerySession *session,
                           const UmiEditorNavigationResult *result)
{
    return strcmp(session->request.origin.uri, result->location.uri) == 0 &&
           session->request.document_revision != 0U &&
           result->location.document_revision != 0U &&
           session->request.document_revision !=
               result->location.document_revision;
}

/* Provide the rank result operation used by this module and its client applications. */
static int32_t rank_result(const UmiEditorNavigationQuerySession *session,
                           const UmiEditorNavigationProviderRegistration *provider,
                           const UmiEditorNavigationResult *result,
                           int stale)
{
    int32_t score = result->rank;
    int64_t provider_score = (int64_t)provider->descriptor.priority * 1000LL;

    score = bounded_add_score(score, provider_score);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result->primary) score = bounded_add_score(score, 5000LL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(result->location.uri, session->request.origin.uri) == 0) {
        score = bounded_add_score(score, 500LL);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->request.flags & UMI_EDITOR_NAVIGATION_REQUEST_PREFER_LOCAL) !=
            0U &&
        (provider->descriptor.flags &
         UMI_EDITOR_NAVIGATION_PROVIDER_REMOTE) == 0U) {
        score = bounded_add_score(score, 250LL);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (stale) score = bounded_add_score(score, -100000LL);
    return score;
}

/* Provide the accept result operation used by this module and its client applications. */
static UmiStatus accept_result(QuerySinkContext *context,
                               const UmiEditorNavigationResult *source)
{
    UmiEditorNavigationQuerySession *session;
    UmiEditorNavigationResult result;
    size_t index;
    int stale;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = context->session;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || context->provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->cancel_requested) return UMI_STATUS_CANCELLED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!valid_result(source)) return UMI_STATUS_INVALID_ARGUMENT;
    result = *source;
    status = copy_text(result.provider_id, sizeof(result.provider_id),
                       context->provider->descriptor.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    stale = result_is_stale(session, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (stale && !session->options.include_stale_results &&
        (session->request.flags & UMI_EDITOR_NAVIGATION_REQUEST_ALLOW_STALE) ==
            0U) {
        ++session->stale_result_count;
        return UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (stale) ++session->stale_result_count;
    result.rank = rank_result(session, context->provider, &result, stale);
    result.sequence = ++session->sequence;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->result_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_result(&session->results[index], &result)) {
            ++session->duplicate_result_count;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (result.rank > session->results[index].rank ||
                (result.rank == session->results[index].rank && result.primary &&
                 !session->results[index].primary)) {
                session->results[index] = result;
            }
            return UMI_STATUS_OK;
        }
    }
    status = reserve_results(session, session->result_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->results[session->result_count++] = result;
    return UMI_STATUS_OK;
}

/* Provide the result sink operation used by this module and its client applications. */
static UmiStatus result_sink(const UmiEditorNavigationResult *result,
                             void *user_data)
{
    return accept_result((QuerySinkContext *)user_data, result);
}

/* Provide the symbol sink operation used by this module and its client applications. */
static UmiStatus symbol_sink(const UmiEditorIndexedSymbol *symbol,
                             void *user_data)
{
    QuerySinkContext *context = (QuerySinkContext *)user_data;
    UmiEditorNavigationResult result;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (symbol == NULL ||
        symbol->struct_size != (uint32_t)sizeof(*symbol) ||
        symbol->api_version != UMI_EDITOR_SYMBOL_INDEX_API_VERSION ||
        symbol->id[0] == '\0' ||
        !terminated(symbol->id, sizeof(symbol->id)) ||
        !terminated(symbol->name, sizeof(symbol->name)) ||
        !terminated(symbol->detail, sizeof(symbol->detail)) ||
        !terminated(symbol->signature, sizeof(symbol->signature)) ||
        !valid_location(&symbol->location) ||
        (symbol->selection_location.uri[0] != '\0' &&
         !valid_location(&symbol->selection_location))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&result, 0, sizeof(result));
    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    result.location = symbol->selection_location.uri[0] != '\0'
        ? symbol->selection_location
        : symbol->location;
    result.location.kind = UMI_EDITOR_SOURCE_LOCATION_SYMBOL;
    status = copy_text(result.location.symbol_id,
                       sizeof(result.location.symbol_id), symbol->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(result.location.label, sizeof(result.location.label),
                           symbol->name);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(result.detail, sizeof(result.detail),
                           symbol->detail[0] != '\0'
                               ? symbol->detail
                               : symbol->signature);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    result.rank = 0;
    result.primary = 0;
    return accept_result(context, &result);
}

/* Provide the compare results operation used by this module and its client applications. */
static int compare_results(const void *left_pointer,
                           const void *right_pointer)
{
    const UmiEditorNavigationResult *left =
        (const UmiEditorNavigationResult *)left_pointer;
    const UmiEditorNavigationResult *right =
        (const UmiEditorNavigationResult *)right_pointer;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->rank > right->rank) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->rank < right->rank) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->primary != right->primary) return left->primary ? -1 : 1;
    order = strcmp(left->location.uri, right->location.uri);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->location.line < right->location.line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->location.line > right->location.line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->location.column < right->location.column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->location.column > right->location.column) return 1;
    return strcmp(left->provider_id, right->provider_id);
}

/*
 * Provide the has primary result operation used by this module and its client
 * applications.
 */
static int has_primary_result(const UmiEditorNavigationQuerySession *session)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->result_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->results[index].primary) return 1;
    }
    return 0;
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

/*
 * Initialise editor navigation query options from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_query_options_initialize(
    UmiEditorNavigationQueryOptions *options)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(options, 0, sizeof(*options));
    options->struct_size = (uint32_t)sizeof(*options);
    options->api_version = UMI_EDITOR_NAVIGATION_QUERY_SESSION_API_VERSION;
    options->maximum_results = UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    options->allow_partial_results = 1;
    options->include_stale_results = 0;
    options->wrap_selection = 1;
    return UMI_STATUS_OK;
}

/*
 * Initialise editor navigation query session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_query_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationQuerySession **out_session)
{
    UmiEditorNavigationQuerySession *session;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorNavigationQuerySession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->registry = registry;
    session->selected_index = SIZE_MAX;
    session->revision = 1U;
    session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_EMPTY;
    (void)umi_editor_navigation_query_options_initialize(&session->options);
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor navigation query session so the same storage can
 * be reused safely.
 */
void umi_editor_navigation_query_session_destroy(
    UmiEditorNavigationQuerySession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    free(session->results);
    free(session->reports);
    session->results = NULL;
    session->reports = NULL;
    free(session);
}

/*
 * Release or reset state held by editor navigation query session so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_navigation_query_session_clear(
    UmiEditorNavigationQuerySession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session->result_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->stale_result_count = 0U;
    session->duplicate_result_count = 0U;
    session->failed_provider_count = 0U;
    session->cancel_requested = 0;
    session->truncated = 0;
    session->has_request = 0;
    session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_EMPTY;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Perform editor navigation query session through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_navigation_query_session_execute(
    UmiEditorNavigationQuerySession *session,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationQueryOptions *options)
{
    UmiEditorNavigationQueryOptions effective_options;
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorNavigationCancellation cancellation;
    size_t provider_index;
    size_t matched_provider_count = 0U;
    UmiStatus status;
    UmiStatus first_failure = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U ||
        request->query_kind <= UMI_EDITOR_NAVIGATION_QUERY_NONE ||
        request->query_kind > UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL ||
        request->maximum_results > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS ||
        (request->flags &
         ~(UmiEditorNavigationRequestFlags)UMI_EDITOR_NAVIGATION_REQUEST_ALL_FLAGS) != 0U ||
        !valid_location(&request->origin) ||
        !terminated(request->workspace_id, sizeof(request->workspace_id)) ||
        !terminated(request->document_id, sizeof(request->document_id)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        !terminated(request->symbol_id, sizeof(request->symbol_id)) ||
        !terminated(request->text, sizeof(request->text))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = options != NULL ? UMI_STATUS_OK
                             : umi_editor_navigation_query_options_initialize(
                                   &effective_options);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options != NULL) effective_options = *options;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective_options.struct_size != (uint32_t)sizeof(effective_options) ||
        effective_options.api_version !=
            UMI_EDITOR_NAVIGATION_QUERY_SESSION_API_VERSION ||
        effective_options.maximum_results == 0U ||
        effective_options.maximum_results > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    session->result_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->stale_result_count = 0U;
    session->duplicate_result_count = 0U;
    session->failed_provider_count = 0U;
    session->cancel_requested = 0;
    session->truncated = 0;
    session->request = *request;
    session->options = effective_options;
    session->registry_revision =
        umi_editor_navigation_provider_registry_revision(session->registry);
    session->has_request = 1;
    session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_RUNNING;
    session->revision = next_revision(session->revision);

    cancellation.struct_size = (uint32_t)sizeof(cancellation);
    cancellation.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    cancellation.is_cancelled = internal_cancelled;
    cancellation.user_data = session;

    /* Visit each bounded item once so every record receives the same rule. */
    for (provider_index = 0U;
         provider_index <
             umi_editor_navigation_provider_registry_count(session->registry);
         ++provider_index) {
        UmiEditorNavigationProviderReport report;
        QuerySinkContext context;
        UmiStatus provider_status;
        size_t before_count;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_navigation_provider_registry_at(session->registry,
                                                       provider_index,
                                                       &registration) !=
            UMI_STATUS_OK) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_editor_navigation_provider_supports_request(&registration,
                                                             request)) {
            continue;
        }
        ++matched_provider_count;
        status = reserve_reports(session, session->report_count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED;
            session->revision = next_revision(session->revision);
            return status;
        }
        initialise_report(&report, registration.descriptor.id,
                          ++session->sequence);
        context.session = session;
        context.provider = &registration;
        before_count = session->result_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL ||
            request->query_kind == UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL) {
            provider_status = registration.functions.symbols(
                registration.instance, request, &cancellation, symbol_sink,
                &context, &report);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            provider_status = registration.functions.query(
                registration.instance, request, &cancellation, result_sink,
                &context, &report);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (report.struct_size != (uint32_t)sizeof(report) ||
            report.api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
            !terminated(report.message, sizeof(report.message))) {
            initialise_report(&report, registration.descriptor.id,
                              session->sequence);
        }
        (void)copy_text(report.provider_id, sizeof(report.provider_id),
                        registration.descriptor.id);
        report.status = provider_status;
        report.emitted_count = session->result_count - before_count;
        report.cancelled = session->cancel_requested ||
                           provider_status == UMI_STATUS_CANCELLED;
        session->reports[session->report_count++] = report;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (provider_status != UMI_STATUS_OK &&
            provider_status != UMI_STATUS_NOT_FOUND &&
            provider_status != UMI_STATUS_CANCELLED) {
            ++session->failed_provider_count;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (first_failure == UMI_STATUS_OK) first_failure = provider_status;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (!effective_options.allow_partial_results) {
                session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED;
                session->revision = next_revision(session->revision);
                return provider_status;
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (session->cancel_requested || provider_status == UMI_STATUS_CANCELLED) {
            session->cancel_requested = 1;
            break;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (effective_options.stop_on_first_primary && has_primary_result(session)) {
            break;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->result_count > 1U) {
        qsort(session->results, session->result_count,
              sizeof(*session->results), compare_results);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->result_count > effective_options.maximum_results) {
        session->result_count = effective_options.maximum_results;
        session->truncated = 1;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->result_count > 0U) session->selected_index = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->cancel_requested) {
        session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_CANCELLED;
        status = UMI_STATUS_CANCELLED;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (matched_provider_count == 0U) {
        session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED;
        status = UMI_STATUS_NOT_FOUND;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (session->failed_provider_count == matched_provider_count &&
               session->result_count == 0U && first_failure != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED;
        status = first_failure;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_COMPLETE;
        status = UMI_STATUS_OK;
    }
    session->revision = next_revision(session->revision);
    return status;
}

/*
 * Provide the editor navigation query session refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_query_session_refresh(
    UmiEditorNavigationQuerySession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_navigation_query_session_execute(session,
                                                       &session->request,
                                                       &session->options);
}

/*
 * Provide the editor navigation query session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_query_session_cancel(
    UmiEditorNavigationQuerySession *session)
{
    size_t index;
    UmiEditorNavigationProviderRegistration registration;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_NAVIGATION_QUERY_SESSION_RUNNING) {
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
            umi_editor_navigation_provider_supports_request(&registration,
                                                             &session->request) &&
            registration.functions.cancel != NULL) {
            (void)registration.functions.cancel(registration.instance,
                                                session->request.request_id);
        }
    }
    session->state = UMI_EDITOR_NAVIGATION_QUERY_SESSION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation query session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_query_session_select(
    UmiEditorNavigationQuerySession *session,
    size_t position)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->result_count) return UMI_STATUS_NOT_FOUND;
    session->selected_index = position;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation query session select next operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_query_session_select_next(
    UmiEditorNavigationQuerySession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->result_count == 0U) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX) session->selected_index = 0U;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (session->selected_index + 1U < session->result_count) {
        ++session->selected_index;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (session->options.wrap_selection) {
        session->selected_index = 0U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor navigation query session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_query_session_select_previous(
    UmiEditorNavigationQuerySession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->result_count == 0U) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX) session->selected_index = 0U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (session->selected_index > 0U) --session->selected_index;
    else /* Apply this branch only when its contract condition is satisfied. */ if (session->options.wrap_selection) {
        session->selected_index = session->result_count - 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor navigation query session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_at(
    const UmiEditorNavigationQuerySession *session,
    size_t position,
    UmiEditorNavigationResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->result_count) return UMI_STATUS_NOT_FOUND;
    *out_result = session->results[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor navigation query session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_selected(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX ||
        session->selected_index >= session->result_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_result = session->results[session->selected_index];
    return UMI_STATUS_OK;
}

/*
 * Find editor navigation query session provider report while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_provider_report_at(
    const UmiEditorNavigationQuerySession *session,
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
 * Provide the editor navigation query session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_query_session_snapshot(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationQuerySessionSnapshot *out_snapshot)
{
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
        UMI_EDITOR_NAVIGATION_QUERY_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->query_kind = session->has_request
        ? session->request.query_kind
        : UMI_EDITOR_NAVIGATION_QUERY_NONE;
    out_snapshot->result_count = session->result_count;
    out_snapshot->provider_count = session->report_count;
    out_snapshot->failed_provider_count = session->failed_provider_count;
    out_snapshot->stale_result_count = session->stale_result_count;
    out_snapshot->duplicate_result_count = session->duplicate_result_count;
    out_snapshot->selected_index = session->selected_index;
    out_snapshot->request_id = session->has_request
        ? session->request.request_id
        : 0U;
    out_snapshot->registry_revision = session->registry_revision;
    out_snapshot->revision = session->revision;
    out_snapshot->has_selection = session->selected_index != SIZE_MAX &&
                                  session->selected_index < session->result_count;
    out_snapshot->truncated = session->truncated;
    out_snapshot->cancelled = session->cancel_requested;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor navigation query session without
 * changing their state.
 */
size_t umi_editor_navigation_query_session_count(
    const UmiEditorNavigationQuerySession *session)
{
    return session != NULL ? session->result_count : 0U;
}

/*
 * Return the number of records represented by editor navigation query session provider
 * report without changing their state.
 */
size_t umi_editor_navigation_query_session_provider_report_count(
    const UmiEditorNavigationQuerySession *session)
{
    return session != NULL ? session->report_count : 0U;
}

/*
 * Provide the editor navigation query session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_navigation_query_session_revision(
    const UmiEditorNavigationQuerySession *session)
{
    return session != NULL ? session->revision : 0U;
}
