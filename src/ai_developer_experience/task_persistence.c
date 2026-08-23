/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_persistence.c
 *
 * PURPOSE:
 *   Implement bounded recent task persistence through Session Store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_persistence.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus key(
    const char *prefix,
    const char *suffix,
    char *out_key,
    size_t capacity)
{
    int written = snprintf(out_key, capacity, "%s.%s", prefix, suffix);
    return written >= 0 && (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

static UmiStatus save_text_field(
    UmiSessionStore *store,
    const char *prefix,
    const char *field,
    const char *text)
{
    char text_prefix[UMI_SESSION_KEY_CAPACITY];
    char count_key[UMI_SESSION_KEY_CAPACITY];
    size_t chunks = 0U;
    int written;
    UmiStatus status;

    written = snprintf(
        text_prefix,
        sizeof(text_prefix),
        "%s.%s",
        prefix,
        field);
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_ai_developer_persistence_save_text(
        store,
        text_prefix,
        text,
        strlen(text),
        &chunks);
    if (status != UMI_STATUS_OK) return status;

    status = key(text_prefix, "count", count_key, sizeof(count_key));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_persistence_set_uint64(
        store, count_key, chunks);
}

static UmiStatus load_text_field(
    const UmiSessionStore *store,
    const char *prefix,
    const char *field,
    char *out_text,
    size_t capacity)
{
    char text_prefix[UMI_SESSION_KEY_CAPACITY];
    char count_key[UMI_SESSION_KEY_CAPACITY];
    uint64_t chunks = 0U;
    size_t length = 0U;
    int written;
    UmiStatus status;

    written = snprintf(
        text_prefix,
        sizeof(text_prefix),
        "%s.%s",
        prefix,
        field);
    if (written < 0 || (size_t)written >= sizeof(text_prefix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = key(text_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &chunks);
    }
    if (status != UMI_STATUS_OK || chunks == 0U) {
        return status != UMI_STATUS_OK
            ? status : UMI_STATUS_PARSE_ERROR;
    }

    return umi_ai_developer_persistence_load_text(
        store,
        text_prefix,
        (size_t)chunks,
        out_text,
        capacity,
        &length);
}

static UmiStatus save_entry(
    UmiSessionStore *store,
    const char *prefix,
    const UmiAiDeveloperTaskEntry *entry)
{
    char key_text[UMI_SESSION_KEY_CAPACITY];
    UmiStatus status;

    status = save_text_field(store, prefix, "id", entry->task_id);
    if (status == UMI_STATUS_OK) {
        status = save_text_field(
            store, prefix, "request", entry->request_id);
    }
    if (status == UMI_STATUS_OK) {
        status = save_text_field(
            store, prefix, "title", entry->title);
    }
    if (status == UMI_STATUS_OK) {
        status = save_text_field(
            store, prefix, "summary", entry->summary);
    }
    if (status != UMI_STATUS_OK) return status;

#define SAVE_NUMBER(field_name, value) \
    do { \
        status = key(prefix, field_name, key_text, sizeof(key_text)); \
        if (status == UMI_STATUS_OK) { \
            status = umi_ai_developer_persistence_set_uint64( \
                store, key_text, (uint64_t)(value)); \
        } \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)

    SAVE_NUMBER("state", entry->state);
    SAVE_NUMBER("iteration", entry->iteration);
    SAVE_NUMBER("maxIterations", entry->maximum_iterations);
    SAVE_NUMBER("patchFiles", entry->patch_files);
    SAVE_NUMBER("validationFailures", entry->validation_failures);
    SAVE_NUMBER("created", entry->created_sequence);
    SAVE_NUMBER("updated", entry->updated_sequence);

#undef SAVE_NUMBER

    return UMI_STATUS_OK;
}

static UmiStatus load_entry(
    const UmiSessionStore *store,
    const char *prefix,
    UmiAiDeveloperTaskEntry *entry)
{
    char key_text[UMI_SESSION_KEY_CAPACITY];
    uint64_t value = 0U;
    UmiStatus status;

    (void)memset(entry, 0, sizeof(*entry));

    status = load_text_field(
        store, prefix, "id", entry->task_id, sizeof(entry->task_id));
    if (status == UMI_STATUS_OK) {
        status = load_text_field(
            store, prefix, "request",
            entry->request_id, sizeof(entry->request_id));
    }
    if (status == UMI_STATUS_OK) {
        status = load_text_field(
            store, prefix, "title",
            entry->title, sizeof(entry->title));
    }
    if (status == UMI_STATUS_OK) {
        status = load_text_field(
            store, prefix, "summary",
            entry->summary, sizeof(entry->summary));
    }
    if (status != UMI_STATUS_OK) return status;

#define LOAD_NUMBER(field_name, target) \
    do { \
        status = key(prefix, field_name, key_text, sizeof(key_text)); \
        if (status == UMI_STATUS_OK) { \
            status = umi_ai_developer_persistence_get_uint64( \
                store, key_text, 0U, &value); \
        } \
        if (status != UMI_STATUS_OK) return status; \
        target = value; \
    } while (0)

    LOAD_NUMBER("state", value);
    entry->state = (UmiAiDeveloperTaskState)value;
    LOAD_NUMBER("iteration", value);
    entry->iteration = (uint32_t)value;
    LOAD_NUMBER("maxIterations", value);
    entry->maximum_iterations = (uint32_t)value;
    LOAD_NUMBER("patchFiles", value);
    entry->patch_files = (size_t)value;
    LOAD_NUMBER("validationFailures", value);
    entry->validation_failures = (size_t)value;
    LOAD_NUMBER("created", entry->created_sequence);
    LOAD_NUMBER("updated", entry->updated_sequence);

#undef LOAD_NUMBER

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_tasks_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperTaskRegistry *registry)
{
    char count_key[UMI_SESSION_KEY_CAPACITY];
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL || registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    total = umi_ai_developer_task_registry_count(registry);
    count =
        total > UMI_AI_DEVELOPER_PERSISTED_TASKS
            ? UMI_AI_DEVELOPER_PERSISTED_TASKS
            : total;
    first = total - count;

    status = key(key_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_set_uint64(
            store, count_key, count);
    }
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < count; ++index) {
        UmiAiDeveloperTaskEntry entry;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written;

        status = umi_ai_developer_task_registry_at(
            registry, first + index, &entry);
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(
            prefix, sizeof(prefix), "%s.t%zu", key_prefix, index);
        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = save_entry(store, prefix, &entry);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_tasks_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperTaskRegistry *registry,
    size_t *out_restored_count)
{
    char count_key[UMI_SESSION_KEY_CAPACITY];
    uint64_t count = 0U;
    size_t index;
    UmiStatus status;

    if (store == NULL || key_prefix == NULL ||
        registry == NULL || out_restored_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_restored_count = 0U;

    status = key(key_prefix, "count", count_key, sizeof(count_key));
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_persistence_get_uint64(
            store, count_key, 0U, &count);
    }
    if (status != UMI_STATUS_OK) return status;

    if (count > UMI_AI_DEVELOPER_PERSISTED_TASKS) {
        return UMI_STATUS_PARSE_ERROR;
    }

    umi_ai_developer_task_registry_clear(registry);

    for (index = 0U; index < (size_t)count; ++index) {
        UmiAiDeveloperTaskEntry entry;
        char prefix[UMI_SESSION_KEY_CAPACITY];
        int written = snprintf(
            prefix, sizeof(prefix), "%s.t%zu", key_prefix, index);

        if (written < 0 || (size_t)written >= sizeof(prefix)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        status = load_entry(store, prefix, &entry);
        if (status != UMI_STATUS_OK) return status;

        status = umi_ai_developer_task_registry_upsert(
            registry, &entry);
        if (status != UMI_STATUS_OK) return status;

        *out_restored_count += 1U;
    }

    return UMI_STATUS_OK;
}
