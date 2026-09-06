/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/template_store.c
 *
 * PURPOSE:
 *   Implement owned in-memory template storage for starters and designer
 *   component fragments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model. It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/template_store.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiDeclTemplateItem {
    char id[UMI_DECL_ID_CAPACITY];
    char *source;
} UmiDeclTemplateItem;

struct UmiDeclTemplateStore {
    UmiDeclTemplateItem items[UMI_DECL_MAX_TEMPLATES];
    size_t count;
};

/* Treat a damaged private count as an empty tail so cleanup and lookup never
 * walk beyond the fixed array owned by the store. */
static size_t safe_count(const UmiDeclTemplateStore *store)
{
    if (store == NULL) return 0U;
    return store->count <= UMI_DECL_MAX_TEMPLATES
        ? store->count
        : 0U;
}

/*
 * Initialise decl template store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_decl_template_store_create(UmiDeclTemplateStore **out_store)
{
    UmiDeclTemplateStore *store;

    /* Tell the caller that no object exists until allocation succeeds. */
    if (out_store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    store = (UmiDeclTemplateStore *)calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by decl template store so the same storage can be reused
 * safely.
 */
void umi_decl_template_store_destroy(UmiDeclTemplateStore *store)
{
    size_t index;

    if (store == NULL) return;
    /* Use the bounded view of count even if memory was damaged before cleanup. */
    for (index = 0U; index < safe_count(store); ++index) {
        free(store->items[index].source);
    }
    free(store);
}

/*
 * Provide the decl template store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_put(UmiDeclTemplateStore *store,
                                      const char *template_id,
                                      const char *source)
{
    size_t index;
    size_t source_length;
    char *copy;
    UmiStatus status;

    if (store == NULL || !umi_decl_id_is_valid(template_id) ||
        source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* A corrupt count must not be used as an array bound or silently repaired
     * while another item may still own allocated source text. */
    if (store->count > UMI_DECL_MAX_TEMPLATES) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->items[index].id, template_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (store->count >= UMI_DECL_MAX_TEMPLATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    source_length = strlen(source);
    /* Reserve one byte for the terminator without allowing a maximal length
     * to wrap back to a tiny allocation. */
    if (source_length == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    copy = (char *)malloc(source_length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)memcpy(copy, source, source_length + 1U);

    status = umi_decl_copy_text(store->items[store->count].id,
                                sizeof(store->items[store->count].id),
                                template_id);
    if (status != UMI_STATUS_OK) {
        free(copy);
        return status;
    }
    store->items[store->count].source = copy;
    store->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl template store get operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_get(const UmiDeclTemplateStore *store,
                                      const char *template_id,
                                      const char **out_source)
{
    size_t index;

    if (store == NULL || template_id == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_source = NULL;
    if (store->count > UMI_DECL_MAX_TEMPLATES) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->items[index].id, template_id) == 0) {
            if (store->items[index].source == NULL)
                return UMI_STATUS_INVALID_STATE;
            *out_source = store->items[index].source;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by decl template store without changing their
 * state.
 */
size_t umi_decl_template_store_count(const UmiDeclTemplateStore *store)
{
    return safe_count(store);
}
