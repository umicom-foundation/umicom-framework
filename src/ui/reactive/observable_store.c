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
