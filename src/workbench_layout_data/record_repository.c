/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/record_repository.c
 *
 * PURPOSE:
 *   Implement shared Data Server access, prefix enumeration, transaction handling and typed record repository operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/record_repository.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"




/*
 * Copy workbench layout data store into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_layout_data_store_set(
    UmiDataServer *server,
    const char *key,
    const char *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || key == NULL || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(key) >= UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY ||
        strlen(value) >= UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_data_server_set(server, key, value);
}

/*
 * Provide the workbench layout data store get allocated operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_store_get_allocated(
    const UmiDataServer *server,
    const char *key,
    char **out_value,
    size_t *out_size)
{
    char *buffer;
    size_t capacity = UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_value = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_size != NULL) *out_size = 0U;
    buffer = (char *)calloc(capacity, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_data_server_get(server, key, buffer, capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(buffer);
        return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_size != NULL) *out_size = strlen(buffer);
    *out_value = buffer;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data store delete if present operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_store_delete_if_present(
    UmiDataServer *server,
    const char *key)
{
    UmiStatus status = umi_data_server_delete(server, key);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}

/* Provide the visit bridge operation used by this module and its client applications. */
static UmiStatus visit_bridge(
    const char *key,
    const char *value,
    void *user_data)
{
    UmiWorkbenchLayoutDataVisitContext *visit =
        (UmiWorkbenchLayoutDataVisitContext *)user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (visit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    visit->visited += 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_layout_data_key_matches_prefix(
            key, visit->prefix)) {
        return UMI_STATUS_OK;
    }
    visit->accepted += 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (visit->accept == NULL) return UMI_STATUS_OK;
    visit->status = visit->accept(key, value, visit->context);
    return visit->status;
}

/*
 * Provide the workbench layout data store visit prefix operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_store_visit_prefix(
    const UmiDataServer *server,
    const char *prefix,
    UmiStatus (*accept)(const char *key, const char *value, void *context),
    void *context,
    size_t *out_accepted)
{
    UmiWorkbenchLayoutDataVisitContext visit;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || prefix == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&visit, 0, sizeof(visit));
    visit.prefix = prefix;
    visit.accept = accept;
    visit.context = context;
    visit.status = UMI_STATUS_OK;
    status = umi_data_server_visit(server, visit_bridge, &visit);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_accepted != NULL) *out_accepted = visit.accepted;
    return status != UMI_STATUS_OK ? status : visit.status;
}

/*
 * Provide the workbench layout data transaction begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_transaction_begin(
    UmiDataServer *server,
    bool *out_started)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || out_started == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_started = false;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_data_server_in_transaction(server)) return UMI_STATUS_OK;
    status = umi_data_server_begin(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) *out_started = true;
    return status;
}

/*
 * Provide the workbench layout data transaction finish operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_data_transaction_finish(
    UmiDataServer *server,
    bool started,
    UmiStatus operation_status)
{
    UmiStatus finish_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!started) return operation_status;
    finish_status = operation_status == UMI_STATUS_OK
        ? umi_data_server_commit(server)
        : umi_data_server_rollback(server);
    return operation_status != UMI_STATUS_OK
        ? operation_status : finish_status;
}

/*
 * Provide the workbench layout data make record id operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_data_make_record_id(
    const char *prefix,
    const char *aggregate_id,
    uint64_t sequence,
    char *buffer,
    size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (prefix == NULL || aggregate_id == NULL ||
        buffer == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(buffer, capacity, "%s-%s-%020llu",
                       prefix, aggregate_id,
                       (unsigned long long)sequence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= capacity) {
        buffer[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench layout data record repository from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_data_record_repository_init(
    UmiWorkbenchLayoutDataRecordRepository *repository,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind kind,
    size_t record_size,
    UmiWorkbenchLayoutDataRecordEncoder encode,
    UmiWorkbenchLayoutDataRecordDecoder decode)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || server == NULL ||
        record_size == 0U || encode == NULL || decode == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(repository, 0, sizeof(*repository));
    repository->structure_size = sizeof(*repository);
    repository->server = server;
    repository->kind = kind;
    repository->record_size = record_size;
    repository->encode = encode;
    repository->decode = decode;
    return UMI_STATUS_OK;
}

/* Provide the repository key operation used by this module and its client applications. */
static UmiStatus repository_key(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    char *key,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL ||
        repository->structure_size < sizeof(*repository)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_workbench_layout_data_key_build(
        repository->kind, aggregate_id, record_id,
        sequence, 0U, key, capacity);
}

/*
 * Write workbench layout data record repository in its stable representation and report
 * capacity or input failures to the caller.
 */
UmiStatus umi_workbench_layout_data_record_repository_save(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    const void *record)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char value[UMI_WORKBENCH_LAYOUT_DATA_VALUE_CAPACITY];
    size_t required = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = repository_key(repository, aggregate_id, record_id,
                            sequence, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = repository->encode(
        record, value, sizeof(value), &required);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required > sizeof(value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_workbench_layout_data_store_set(
        repository->server, key, value);
}

/*
 * Read workbench layout data record repository into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_workbench_layout_data_record_repository_load(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    void *out_record)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    char *value = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = repository_key(repository, aggregate_id, record_id,
                            sequence, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_layout_data_store_get_allocated(
        repository->server, key, &value, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = repository->decode(value, out_record);
    }
    free(value);
    return status;
}

/*
 * Provide the workbench layout data record repository delete operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_record_repository_delete(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence)
{
    char key[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = repository_key(repository, aggregate_id, record_id,
                            sequence, key, sizeof(key));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_data_server_delete(repository->server, key);
}

typedef struct RepositoryListContext {
    const UmiWorkbenchLayoutDataRecordRepository *repository;
    UmiWorkbenchLayoutDataRecordPredicate predicate;
    void *predicate_context;
    UmiWorkbenchLayoutDataRecordPage *page;
    void *decoded_record;
} RepositoryListContext;

/* Provide the repository accept operation used by this module and its client applications. */
static UmiStatus repository_accept(
    const char *key,
    const char *value,
    void *context)
{
    RepositoryListContext *list = (RepositoryListContext *)context;
    unsigned char *destination;
    bool matches = true;
    UmiStatus status;
    (void)key;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || list->page == NULL ||
        list->decoded_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(list->decoded_record, 0,
                 list->repository->record_size);
    status = list->repository->decode(value, list->decoded_record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list->predicate != NULL) {
        status = list->predicate(
            list->decoded_record, list->predicate_context, &matches);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!matches) return UMI_STATUS_OK;

    list->page->total_available += 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->page->count >= list->page->capacity) {
        list->page->truncated = true;
        return UMI_STATUS_OK;
    }
    destination = (unsigned char *)list->page->records +
        list->page->count * list->page->record_size;
    (void)memcpy(destination, list->decoded_record,
                 list->page->record_size);
    list->page->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout data record repository list operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_record_repository_list(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutDataRecordPage *page)
{
    char prefix[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    RepositoryListContext context;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || page == NULL ||
        page->structure_size < sizeof(*page) ||
        page->records == NULL ||
        page->record_size != repository->record_size ||
        page->capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    page->count = 0U;
    page->total_available = 0U;
    page->truncated = false;
    status = umi_workbench_layout_data_key_prefix(
        repository->kind, aggregate_id, prefix, sizeof(prefix));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&context, 0, sizeof(context));
    context.repository = repository;
    context.predicate = predicate;
    context.predicate_context = predicate_context;
    context.page = page;
    context.decoded_record = calloc(1U, repository->record_size);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context.decoded_record == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = umi_workbench_layout_data_store_visit_prefix(
        repository->server, prefix, repository_accept, &context, NULL);
    free(context.decoded_record);
    return status;
}
