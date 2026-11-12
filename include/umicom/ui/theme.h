/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/theme.h
 *
 * PURPOSE:
 *   Define semantic theme tokens so product code does not depend on toolkit-specific
 *   CSS selectors, colours or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_THEME_H
#define UMICOM_UI_THEME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"
#include "umicom/ui/value.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_THEME_MAX 256U

/**
 * Represent the ui theme snapshot data shared with callers of this public contract.
 */
/* All inline fields must terminate within their declared arrays. token_id uses
 * the shared UI identifier grammar. Empty values and descriptions are allowed;
 * storage admission does not validate colour, font, CSS, asset paths or UTF-8. */
typedef struct UmiUiThemeSnapshot {
    char token_id[UMI_UI_ID_CAPACITY];
    char value[UMI_UI_VALUE_STRING_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
} UmiUiThemeSnapshot;

/**
 * Represent the ui theme model data shared with callers of this public contract.
 */
typedef struct UmiUiThemeModel UmiUiThemeModel;

/**
 * Initialise ui theme model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_theme_model_create(UmiUiThemeModel **out_model);
/**
 * Release or reset state held by ui theme model so the same storage can be reused safely.
 */
void umi_ui_theme_model_destroy(UmiUiThemeModel *model);
/**
 * Provide the ui theme model upsert operation used by this module and its client
 * applications.
 */
/* Capture and validate a complete caller-owned snapshot before mutation.
 * Malformed fields return INVALID_ARGUMENT without changing items or revision.
 * Replacing an existing token remains permitted when the catalogue is full.
 * item must not overlap the opaque model allocation. */
UmiStatus umi_ui_theme_model_upsert(UmiUiThemeModel *model,
                                     const UmiUiThemeSnapshot *item);
/**
 * Remove ui theme model while keeping the remaining records in a valid and discoverable
 * state.
 */
/* Queries share umi_ui_id_is_valid: readable through NUL or
 * UMI_UI_ID_CAPACITY bytes. Invalid/unterminated queries return INVALID_ARGUMENT;
 * a valid absent ID returns NOT_FOUND. Refusal leaves the model unchanged. */
UmiStatus umi_ui_theme_model_remove(UmiUiThemeModel *model, const char *item_id);
/**
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
/* The same query rules apply here. Output is a caller-owned value copy and is
 * unchanged on any refusal; it must not overlap the opaque model allocation.
 * A query inside a separate output snapshot is allowed. Callers synchronize
 * their own input/output storage and destruction; the mutex protects model state. */
UmiStatus umi_ui_theme_model_find(const UmiUiThemeModel *model, const char *item_id,
                                   UmiUiThemeSnapshot *out_item);
/**
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Output ownership and refusal preservation are the same as find(). Indices
 * describe the current ordered catalogue and can change after removal. */
UmiStatus umi_ui_theme_model_at(const UmiUiThemeModel *model, size_t index,
                                 UmiUiThemeSnapshot *out_item);
/**
 * Return the number of records represented by ui theme model without changing their state.
 */
size_t umi_ui_theme_model_count(const UmiUiThemeModel *model);
/**
 * Provide the ui theme model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_theme_model_revision(const UmiUiThemeModel *model);

#ifdef __cplusplus
}
#endif

#endif
