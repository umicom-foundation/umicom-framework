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

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiContextStore *store, const char *key)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].key, key) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the set value operation used by this module and its client applications. */
static UmiStatus set_value(UmiUiContextStore *store,
                           const UmiUiContextSnapshot *value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || value == NULL || value->key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, value->key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Initialise ui context store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_context_store_create(UmiUiContextStore **out_store)
{
    UmiUiContextStore *store;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiUiContextStore *)calloc(1U, sizeof(*store));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&store->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(store);
        return status;
    }
    store->revision = 1U;
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ui context store so the same storage can be reused
 * safely.
 */
void umi_ui_context_store_destroy(UmiUiContextStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    umi_mutex_destroy(store->mutex);
    free(store);
}

/*
 * Provide the ui context set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_boolean(UmiUiContextStore *store,
                                     const char *key,
                                     int value)
{
    UmiUiContextSnapshot item = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    item.kind = UMI_UI_CONTEXT_BOOLEAN;
    item.boolean_value = value != 0;
    return set_value(store, &item);
}

/*
 * Provide the ui context set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_integer(UmiUiContextStore *store,
                                     const char *key,
                                     int64_t value)
{
    UmiUiContextSnapshot item = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    item.kind = UMI_UI_CONTEXT_INTEGER;
    item.integer_value = value;
    return set_value(store, &item);
}

/*
 * Provide the ui context set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_string(UmiUiContextStore *store,
                                    const char *key,
                                    const char *value)
{
    UmiUiContextSnapshot item = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key == NULL || key[0] == '\0' || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_ui_copy_text(item.key, sizeof(item.key), key);
    (void)umi_ui_copy_text(item.string_value, sizeof(item.string_value), value);
    item.kind = UMI_UI_CONTEXT_STRING;
    return set_value(store, &item);
}

/* Provide the ui context unset operation used by this module and its client applications. */
UmiStatus umi_ui_context_unset(UmiUiContextStore *store, const char *key)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Provide the ui context get operation used by this module and its client applications. */
UmiStatus umi_ui_context_get(const UmiUiContextStore *store,
                             const char *key,
                             UmiUiContextSnapshot *out_value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(store->mutex);
    index = find_item(store, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(store->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_value = store->items[index];
    (void)umi_mutex_unlock(store->mutex);
    return UMI_STATUS_OK;
}

/* Return the number of records represented by ui context without changing their state. */
size_t umi_ui_context_count(const UmiUiContextStore *store)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        (void)umi_mutex_lock(store->mutex);
        count = store->count;
        (void)umi_mutex_unlock(store->mutex);
    }
    return count;
}

/*
 * Provide the ui context revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_context_revision(const UmiUiContextStore *store)
{
    uint64_t revision = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        (void)umi_mutex_lock(store->mutex);
        revision = store->revision;
        (void)umi_mutex_unlock(store->mutex);
    }
    return revision;
}

/* Copy trim into module-owned storage so callers keep ownership of their input values. */
static void trim_copy(const char *begin, const char *end,
                      char *out, size_t capacity)
{
    size_t length;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (begin < end && isspace((unsigned char)*begin)) ++begin;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (end > begin && isspace((unsigned char)end[-1])) --end;
    length = (size_t)(end - begin);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    (void)memcpy(out, begin, length);
    out[length] = '\0';
}

/* Provide the value as text operation used by this module and its client applications. */
static int value_as_text(const UmiUiContextSnapshot *value,
                         char *out, size_t capacity)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_UI_CONTEXT_BOOLEAN) {
        return snprintf(out, capacity, "%s",
                        value->boolean_value ? "true" : "false") >= 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_UI_CONTEXT_INTEGER) {
        return snprintf(out, capacity, "%" PRId64, value->integer_value) >= 0;
    }
    return umi_ui_copy_text(out, capacity, value->string_value);
}

/* Provide the truthy operation used by this module and its client applications. */
static int truthy(const UmiUiContextSnapshot *value)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_UI_CONTEXT_BOOLEAN) return value->boolean_value != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value->kind == UMI_UI_CONTEXT_INTEGER) return value->integer_value != 0;
    return value->string_value[0] != '\0';
}

/* Provide the evaluate term operation used by this module and its client applications. */
static int evaluate_term(const UmiUiContextStore *store, const char *term)
{
    const char *operator_pos;
    const char *operator_text = NULL;
    char key[UMI_UI_CONTEXT_KEY_CAPACITY];
    char expected[UMI_UI_TEXT_CAPACITY];
    char actual[UMI_UI_TEXT_CAPACITY];
    UmiUiContextSnapshot value;
    int negate = 0;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*term != '\0' && isspace((unsigned char)*term)) ++term;
    /* Apply this branch only when its contract condition is satisfied. */
    if (*term == '!') {
        const char *possible = strstr(term, "!=");
        /* Apply this branch only when its contract condition is satisfied. */
        if (possible != term) {
            negate = 1;
            ++term;
        }
    }

    operator_pos = strstr(term, "!=");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operator_pos != NULL) {
        operator_text = "!=";
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        operator_pos = strchr(term, '=');
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (operator_pos != NULL) operator_text = "=";
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operator_pos == NULL) {
        trim_copy(term, term + strlen(term), key, sizeof(key));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_ui_context_get(store, key, &value) != UMI_STATUS_OK) return 0;
        return negate ? !truthy(&value) : truthy(&value);
    }

    trim_copy(term, operator_pos, key, sizeof(key));
    trim_copy(operator_pos + strlen(operator_text),
              term + strlen(term), expected, sizeof(expected));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_context_get(store, key, &value) != UMI_STATUS_OK) return 0;
    (void)value_as_text(&value, actual, sizeof(actual));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(operator_text, "!=") == 0) return strcmp(actual, expected) != 0;
    return strcmp(actual, expected) == 0;
}

/*
 * Provide the ui context evaluate operation used by this module and its client
 * applications.
 */
int umi_ui_context_evaluate(const UmiUiContextStore *store,
                            const char *expression)
{
    const char *cursor;
    const char *next;
    char term[256];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (expression == NULL || expression[0] == '\0') return 1;
    cursor = expression;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        next = strstr(cursor, "&&");
        trim_copy(cursor,
                  next != NULL ? next : cursor + strlen(cursor),
                  term, sizeof(term));
        /* Apply this branch only when its contract condition is satisfied. */
        if (term[0] == '\0' || !evaluate_term(store, term)) return 0;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (next == NULL) break;
        cursor = next + 2;
    }
    return 1;
}
