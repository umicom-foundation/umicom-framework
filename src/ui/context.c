/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/context.c
 *
 * PURPOSE:
 *   Implement typed context keys and deliberately small conditional-expression evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Expressions are intentionally simpler than a programming language. They are configuration data used to enable or disable UI contributions.
 */

#include "umicom/ui/context.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiContextStore {
    UmiUiContextSnapshot items[UMI_UI_CONTEXT_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static size_t find_item(const UmiUiContextStore *store, const char *key)
{
    size_t index;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->items[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

static UmiStatus set_value(UmiUiContextStore *store,
                           const UmiUiContextSnapshot *value)
{
    size_t index;
    if (store == NULL || value == NULL || value->key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, value->key);
    if (index == SIZE_MAX) {
        if (store->count >= UMI_UI_CONTEXT_MAX) {
            (void)umi_mutex_unlock(store->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = store->count++;
    }
    store->items[index] = *value;
    store->revision = umi_ui_next_revision(store->revision);
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_context_store_create(UmiUiContextStore **out_store)
{
    UmiUiContextStore *store;
    UmiStatus status;
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiUiContextStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&store->mutex);
    if (status != UMI_STATUS_OK) {
        free(store);
        return status;
    }
    store->revision = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_ui_context_store_destroy(UmiUiContextStore *store)
{
    if (store == NULL) return;
    umi_mutex_destroy(store->mutex);
    free(store);
}

UmiStatus umi_ui_context_set_boolean(UmiUiContextStore *store,
                                     const char *key,
                                     int value)
{
    UmiUiContextSnapshot item = {0};
    if (key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    item.kind = UMI_UI_CONTEXT_BOOLEAN;
    item.boolean_value = value != 0;
    return set_value(store, &item);
}

UmiStatus umi_ui_context_set_integer(UmiUiContextStore *store,
                                     const char *key,
                                     int64_t value)
{
    UmiUiContextSnapshot item = {0};
    if (key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    item.kind = UMI_UI_CONTEXT_INTEGER;
    item.integer_value = value;
    return set_value(store, &item);
}

UmiStatus umi_ui_context_set_string(UmiUiContextStore *store,
                                    const char *key,
                                    const char *value)
{
    UmiUiContextSnapshot item = {0};
    if (key == NULL || key[0] == '\0' || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    (void)umi_ui_copy_text(item.string_value, sizeof(item.string_value), value);
    item.kind = UMI_UI_CONTEXT_STRING;
    return set_value(store, &item);
}

UmiStatus umi_ui_context_unset(UmiUiContextStore *store, const char *key)
{
    size_t index;
    if (store == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, key);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < store->count) {
        (void)memmove(&store->items[index],
                      &store->items[index + 1U],
                      (store->count - index - 1U) * sizeof(store->items[0]));
    }
    --store->count;
    store->revision = umi_ui_next_revision(store->revision);
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_context_get(const UmiUiContextStore *store,
                             const char *key,
                             UmiUiContextSnapshot *out_value)
{
    size_t index;
    if (store == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, key);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_value = store->items[index];
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_context_count(const UmiUiContextStore *store)
{
    size_t count = 0U;
    if (store != NULL) {
        (void)umi_mutex_lock(store->mutex);
        count = store->count;
        (void)umi_mutex_unlock(store->mutex);
    }
    return count;
}

uint64_t umi_ui_context_revision(const UmiUiContextStore *store)
{
    uint64_t revision = 0U;
    if (store != NULL) {
        (void)umi_mutex_lock(store->mutex);
        revision = store->revision;
        (void)umi_mutex_unlock(store->mutex);
    }
    return revision;
}

static void trim_copy(const char *begin, const char *end,
                      char *out, size_t capacity)
{
    size_t length;
    while (begin < end && isspace((unsigned char)*begin)) ++begin;
    while (end > begin && isspace((unsigned char)end[-1])) --end;
    length = (size_t)(end - begin);
    if (capacity == 0U) return;
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(out, begin, length);
    out[length] = '\0';
}

static int value_as_text(const UmiUiContextSnapshot *value,
                         char *out, size_t capacity)
{
    if (value->kind == UMI_UI_CONTEXT_BOOLEAN) {
        return snprintf(out, capacity, "%s",
                        value->boolean_value ? "true" : "false") >= 0;
    }
    if (value->kind == UMI_UI_CONTEXT_INTEGER) {
        return snprintf(out, capacity, "%" PRId64, value->integer_value) >= 0;
    }
    return umi_ui_copy_text(out, capacity, value->string_value);
}

static int truthy(const UmiUiContextSnapshot *value)
{
    if (value->kind == UMI_UI_CONTEXT_BOOLEAN) return value->boolean_value != 0;
    if (value->kind == UMI_UI_CONTEXT_INTEGER) return value->integer_value != 0;
    return value->string_value[0] != '\0';
}

static int evaluate_term(const UmiUiContextStore *store, const char *term)
{
    const char *operator_pos;
    const char *operator_text = NULL;
    char key[UMI_UI_CONTEXT_KEY_CAPACITY];
    char expected[UMI_UI_TEXT_CAPACITY];
    char actual[UMI_UI_TEXT_CAPACITY];
    UmiUiContextSnapshot value;
    int negate = 0;

    while (*term != '\0' && isspace((unsigned char)*term)) ++term;
    if (*term == '!') {
        const char *possible = strstr(term, "!=");
        if (possible != term) {
            negate = 1;
            ++term;
        }
    }

    operator_pos = strstr(term, "!=");
    if (operator_pos != NULL) {
        operator_text = "!=";
    } else {
        operator_pos = strchr(term, '=');
        if (operator_pos != NULL) operator_text = "=";
    }

    if (operator_pos == NULL) {
        trim_copy(term, term + strlen(term), key, sizeof(key));
        if (umi_ui_context_get(store, key, &value) != UMI_STATUS_OK) return 0;
        return negate ? !truthy(&value) : truthy(&value);
    }

    trim_copy(term, operator_pos, key, sizeof(key));
    trim_copy(operator_pos + strlen(operator_text),
              term + strlen(term), expected, sizeof(expected));
    if (umi_ui_context_get(store, key, &value) != UMI_STATUS_OK) return 0;
    (void)value_as_text(&value, actual, sizeof(actual));
    if (strcmp(operator_text, "!=") == 0) return strcmp(actual, expected) != 0;
    return strcmp(actual, expected) == 0;
}

int umi_ui_context_evaluate(const UmiUiContextStore *store,
                            const char *expression)
{
    const char *cursor;
    const char *next;
    char term[256];
    if (store == NULL) return 0;
    if (expression == NULL || expression[0] == '\0') return 1;
    cursor = expression;
    while (*cursor != '\0') {
        next = strstr(cursor, "&&");
        trim_copy(cursor,
                  next != NULL ? next : cursor + strlen(cursor),
                  term, sizeof(term));
        if (term[0] == '\0' || !evaluate_term(store, term)) return 0;
        if (next == NULL) break;
        cursor = next + 2;
    }
    return 1;
}
