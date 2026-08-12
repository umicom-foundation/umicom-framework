/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/context.h
 *
 * PURPOSE:
 *   Provide small typed context keys for conditional commands, menus and keybindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef enum UmiUiContextKind {
    UMI_UI_CONTEXT_BOOLEAN = 1,
    UMI_UI_CONTEXT_INTEGER = 2,
    UMI_UI_CONTEXT_STRING = 3
} UmiUiContextKind;

typedef struct UmiUiContextSnapshot {
    char key[UMI_UI_CONTEXT_KEY_CAPACITY];
    UmiUiContextKind kind;
    int boolean_value;
    int64_t integer_value;
    char string_value[UMI_UI_TEXT_CAPACITY];
} UmiUiContextSnapshot;

typedef struct UmiUiContextStore UmiUiContextStore;

UmiStatus umi_ui_context_store_create(UmiUiContextStore **out_store);
void umi_ui_context_store_destroy(UmiUiContextStore *store);
UmiStatus umi_ui_context_set_boolean(UmiUiContextStore *store,
                                     const char *key,
                                     int value);
UmiStatus umi_ui_context_set_integer(UmiUiContextStore *store,
                                     const char *key,
                                     int64_t value);
UmiStatus umi_ui_context_set_string(UmiUiContextStore *store,
                                    const char *key,
                                    const char *value);
UmiStatus umi_ui_context_unset(UmiUiContextStore *store, const char *key);
UmiStatus umi_ui_context_get(const UmiUiContextStore *store,
                             const char *key,
                             UmiUiContextSnapshot *out_value);
size_t umi_ui_context_count(const UmiUiContextStore *store);
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
