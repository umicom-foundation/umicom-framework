/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/observable_store.c
 *
 * PURPOSE:
 *   Hold bounded revisioned UI state keyed by canonical property paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_store.h"
#include <stdlib.h>
#include <string.h>

/* The store is a public value object, so inspect every active row before a
 * lookup. Counts and terminators must be bounded before indexing or strcmp.
 * Callers serialize the complete operation; this is not a synchronization lock. */
static int UmiReactiveStoreStateReadable(const UmiUiReactiveObservableStore *store)
{
    if (store->count > UMI_UI_REACTIVE_MAX_ITEMS) return 0;
    for (size_t i = 0U; i < store->count; ++i) {
        const UmiUiReactiveKeyValue *row = &store->items[i];
        if (row->key[0] == '\0' || memchr(row->key, '\0', sizeof(row->key)) == NULL ||
            !umi_ui_value_valid(&row->value)) return 0;
    }
    return 1;
}
static int UmiReactiveStoreKeyLength(const char *key, size_t *length)
{
    size_t used = 0U;
    if (key == NULL || key[0] == '\0') return 0;
    while (used < UMI_UI_REACTIVE_PATH_CAPACITY && key[used] != '\0') ++used;
    if (used == UMI_UI_REACTIVE_PATH_CAPACITY) return 0;
    *length = used;
    return 1;
}

/* Initialise an empty observable state store. */
void umi_ui_reactive_observable_store_init(UmiUiReactiveObservableStore *s){/* Apply this operation only while the related capability or state is available. */ if(s)memset(s,0,sizeof *s);}
/* Upsert one value by path and advance revisions only when state is accepted. */
/* Establish bounded state and shared value admission before reactive store access. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *s,const char *key,const UmiUiValue *value){size_t i,n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!key||!value||key[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<s->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->items[i].key,key)==0){s->items[i].value=*value;s->items[i].revision=++s->revision;return UMI_STATUS_OK;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->count>=UMI_UI_REACTIVE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;n=strlen(key);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>=UMI_UI_REACTIVE_PATH_CAPACITY)return UMI_STATUS_INVALID_ARGUMENT;memcpy(s->items[s->count].key,key,n+1U);s->items[s->count].value=*value;s->items[s->count].revision=++s->revision;s->count++;return UMI_STATUS_OK;}
#endif
UmiStatus umi_ui_reactive_observable_store_put(UmiUiReactiveObservableStore *store,
    const char *key, const UmiUiValue *value)
{
    UmiUiValue candidate;
    size_t length;
    size_t index;
    if (store == NULL || value == NULL || !UmiReactiveStoreKeyLength(key, &length))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ui_value_copy(&candidate, value) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiReactiveStoreStateReadable(store)) return UMI_STATUS_INVALID_STATE;
    if (store->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    for (index = 0U; index < store->count; ++index)
        if (strcmp(store->items[index].key, key) == 0) break;
    if (index == store->count) {
        if (store->count == UMI_UI_REACTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(store->items[index].key, key, length + 1U);
        ++store->count;
    }
    /* The remaining publication cannot fail. Stage input first so replacing
     * a row from another borrowed row never destroys its source prematurely.
     * Revision exhaustion is a refusal, never a recycled observation identity. */
    store->items[index].value = candidate;
    store->items[index].revision = ++store->revision;
    return UMI_STATUS_OK;
}
/* Read a stored value without transferring ownership. */
/* Establish bounded state and shared value admission before reactive store access. The previous implementation is retained for review. */
#if 0
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *s,const char *key,UmiUiValue *out){size_t i;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!key||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<s->count;i++)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(s->items[i].key,key)==0){*out=s->items[i].value;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
#endif
UmiStatus umi_ui_reactive_observable_store_get(const UmiUiReactiveObservableStore *store,
    const char *key, UmiUiValue *out_value)
{
    size_t length;
    if (store == NULL || out_value == NULL || !UmiReactiveStoreKeyLength(key, &length))
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)length;
    if (!UmiReactiveStoreStateReadable(store)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < store->count; ++i)
        if (strcmp(store->items[i].key, key) == 0)
            return umi_ui_value_copy(out_value, &store->items[i].value);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_ui_reactive_observable_store_apply(UmiUiReactiveObservableStore *store,
    UmiUiReactiveChangeTransaction *transaction, const UmiUiReactiveChangeEvent *events,
    size_t count, UmiUiReactiveBatchUpdate *out_batch)
{
    if (store == NULL || transaction == NULL || out_batch == NULL ||
        (count != 0U && events == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_UI_REACTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (transaction->state != UMI_UI_REACTIVE_TRANSACTION_ACTIVE ||
        transaction->transaction_id[0] == '\0' ||
        memchr(transaction->transaction_id, '\0', sizeof transaction->transaction_id) == NULL ||
        transaction->base_revision != store->revision || !UmiReactiveStoreStateReadable(store))
        return UMI_STATUS_INVALID_STATE;
    if (count != 0U && store->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Build a small publication plan before mutating the live store. Staging
     * only affected rows keeps independent client edits off the owner stack. */
    struct {
        size_t index;
        UmiUiValue value;
    } *changes = count == 0U ? NULL : malloc(count * sizeof *changes);
    if (count != 0U && changes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    bool seen[UMI_UI_REACTIVE_MAX_ITEMS] = {false};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i) {
        const UmiUiReactiveChangeEvent *event = &events[i];
        if (!umi_ui_reactive_change_event_valid(event) || event->path[0] == '\0') {
            status = UMI_STATUS_INVALID_ARGUMENT;
            goto done;
        }
        size_t index = SIZE_MAX;
        for (size_t row = 0U; row < store->count; ++row) {
            if (strcmp(store->items[row].key, event->path) != 0) continue;
            if (index != SIZE_MAX) {
                status = UMI_STATUS_INVALID_STATE;
                goto done;
            }
            index = row;
        }
        if (index == SIZE_MAX) {
            status = UMI_STATUS_NOT_FOUND;
            goto done;
        }
        if (seen[index]) {
            status = UMI_STATUS_ALREADY_EXISTS;
            goto done;
        }
        if (!umi_ui_value_equal(&store->items[index].value, &event->before_value)) {
            status = UMI_STATUS_INVALID_STATE;
            goto done;
        }
        changes[i].index = index;
        status = umi_ui_value_copy(&changes[i].value, &event->after_value);
        if (status != UMI_STATUS_OK) goto done;
        seen[index] = true;
    }

    UmiUiReactiveChangeTransaction completed = *transaction;
    completed.change_count = count;
    status = umi_ui_reactive_change_transaction_commit(&completed);
    if (status != UMI_STATUS_OK) goto done;
    const uint64_t published = store->revision + (count != 0U ? 1U : 0U);
    const UmiUiReactiveBatchUpdate batch = {.mutation_count = count,
        .start_revision = store->revision, .end_revision = published, .committed = true};
    for (size_t i = 0U; i < count; ++i) {
        UmiUiReactiveKeyValue *row = &store->items[changes[i].index];
        row->value = changes[i].value;
        row->revision = published;
    }
    store->revision = published;
    *transaction = completed;
    *out_batch = batch;
done:
    free(changes);
    return status;
}

/* Project the active value to detached storage. Inactive fields may contain
 * previous client data because the store accepts complete legacy value records.
 * A view only receives the selected key, revision and currently active payload. */
static void CopyViewRow(UmiUiReactiveKeyValue *destination, const UmiUiReactiveKeyValue *source)
{
    memset(destination, 0, sizeof *destination);
    memcpy(destination->key, source->key, strlen(source->key) + 1U);
    destination->revision = source->revision;
    destination->value.kind = source->value.kind;
    switch (source->value.kind) {
        case UMI_UI_VALUE_NONE: break;
        case UMI_UI_VALUE_BOOLEAN: destination->value.boolean_value = source->value.boolean_value; break;
        case UMI_UI_VALUE_INTEGER: destination->value.integer_value = source->value.integer_value; break;
        case UMI_UI_VALUE_REAL: destination->value.real_value = source->value.real_value; break;
        case UMI_UI_VALUE_STRING:
            memcpy(destination->value.string_value, source->value.string_value,
                strlen(source->value.string_value) + 1U);
            break;
    }
}

UmiStatus umi_ui_reactive_observable_store_read_changes(const UmiUiReactiveObservableStore *store,
    uint64_t known_revision, const char *const *keys, size_t key_count,
    UmiUiReactiveKeyValue *rows, size_t capacity, UmiUiReactiveStoreRead *out_read)
{
    if (store == NULL || out_read == NULL || (key_count != 0U && keys == NULL) ||
        (capacity != 0U && rows == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (key_count > UMI_UI_REACTIVE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (known_revision > store->revision || !UmiReactiveStoreStateReadable(store))
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < store->count; ++i)
        if (store->items[i].revision == 0U || store->items[i].revision > store->revision)
            return UMI_STATUS_INVALID_STATE;

    /* Resolve the view's selection first. A selected key must have one owner
     * row, and no request can publish part of a rendering update. */
    bool seen[UMI_UI_REACTIVE_MAX_ITEMS] = {false};
    size_t selected[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t changed = 0U;
    for (size_t i = 0U; i < key_count; ++i) {
        size_t length, index = SIZE_MAX;
        if (!UmiReactiveStoreKeyLength(keys[i], &length)) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t candidate = 0U; candidate < store->count; ++candidate) {
            if (strncmp(store->items[candidate].key, keys[i], length + 1U) != 0) continue;
            if (index != SIZE_MAX) return UMI_STATUS_INVALID_STATE;
            index = candidate;
        }
        if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
        if (seen[index]) return UMI_STATUS_ALREADY_EXISTS;
        seen[index] = true;
        if (store->items[index].revision > known_revision) selected[changed++] = index;
    }
    if (changed > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < changed; ++i) CopyViewRow(&rows[i], &store->items[selected[i]]);
    *out_read = (UmiUiReactiveStoreRead){.observed_revision = store->revision, .row_count = changed};
    return UMI_STATUS_OK;
}
