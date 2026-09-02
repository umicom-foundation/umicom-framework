/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/context.h
 *
 * PURPOSE:
 *   Provide small typed context keys for conditional commands, menus and keybindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Context keys answer questions such as 'is a workspace open?' without allowing presentation code to inspect private application state.
 */

#ifndef UMICOM_UI_CONTEXT_H
#define UMICOM_UI_CONTEXT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_CONTEXT_MAX 256U
#define UMI_UI_CONTEXT_KEY_CAPACITY 160U

/**
 * List the named ui context kind values accepted by this public contract.
 */
typedef enum UmiUiContextKind {
    UMI_UI_CONTEXT_BOOLEAN = 1,
    UMI_UI_CONTEXT_INTEGER = 2,
    UMI_UI_CONTEXT_STRING = 3
} UmiUiContextKind;

/**
 * Represent the ui context snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiContextSnapshot {
    char key[UMI_UI_CONTEXT_KEY_CAPACITY];
    UmiUiContextKind kind;
    int boolean_value;
    int64_t integer_value;
    char string_value[UMI_UI_TEXT_CAPACITY];
} UmiUiContextSnapshot;

/**
 * Represent the ui context store data shared with callers of this public contract.
 */
typedef struct UmiUiContextStore UmiUiContextStore;

/**
 * Initialise ui context store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_context_store_create(UmiUiContextStore **out_store);
/**
 * Release or reset state held by ui context store so the same storage can be reused
 * safely.
 */
void umi_ui_context_store_destroy(UmiUiContextStore *store);
/**
 * Provide the ui context set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_boolean(UmiUiContextStore *store,
                                     const char *key,
                                     int value);
/**
 * Provide the ui context set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_integer(UmiUiContextStore *store,
                                     const char *key,
                                     int64_t value);
/**
 * Provide the ui context set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_context_set_string(UmiUiContextStore *store,
                                    const char *key,
                                    const char *value);
/**
 * Provide the ui context unset operation used by this module and its client applications.
 */
UmiStatus umi_ui_context_unset(UmiUiContextStore *store, const char *key);
/**
 * Provide the ui context get operation used by this module and its client applications.
 */
UmiStatus umi_ui_context_get(const UmiUiContextStore *store,
                             const char *key,
                             UmiUiContextSnapshot *out_value);
/**
 * Return the number of records represented by ui context without changing their state.
 */
size_t umi_ui_context_count(const UmiUiContextStore *store);
/**
 * Provide the ui context revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_context_revision(const UmiUiContextStore *store);

/*
 * Supported expressions are intentionally small and deterministic:
 *   key
 *   !key
 *   key=value
 *   key!=value
 *   expression && expression
 */
int umi_ui_context_evaluate(const UmiUiContextStore *store,
                            const char *expression);

#ifdef __cplusplus
}
#endif
#endif
