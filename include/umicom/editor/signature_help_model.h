/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/signature_help_model.h
 *
 * PURPOSE:
 *   Model callable overloads, signature-help trigger context, active overload
 *   selection and visibility for every Umicom editor frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SIGNATURE_HELP_MODEL_H
#define UMICOM_EDITOR_SIGNATURE_HELP_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SIGNATURE_HELP_MODEL_API_VERSION 1U
#define UMI_EDITOR_SIGNATURE_ID_CAPACITY 128U
#define UMI_EDITOR_SIGNATURE_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_SIGNATURE_LABEL_CAPACITY 1024U
#define UMI_EDITOR_SIGNATURE_DOCUMENTATION_CAPACITY 4096U
#define UMI_EDITOR_SIGNATURE_RETURN_TYPE_CAPACITY 256U

/**
 * List the named editor signature help trigger kind values accepted by this public
 * contract.
 */
typedef enum UmiEditorSignatureHelpTriggerKind {
    UMI_EDITOR_SIGNATURE_HELP_INVOKED = 1,
    UMI_EDITOR_SIGNATURE_HELP_TRIGGER_CHARACTER = 2,
    UMI_EDITOR_SIGNATURE_HELP_CONTENT_CHANGE = 3,
    UMI_EDITOR_SIGNATURE_HELP_RETRIGGER = 4
} UmiEditorSignatureHelpTriggerKind;

/**
 * Represent the editor signature help item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSignatureHelpItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SIGNATURE_ID_CAPACITY];
    char provider_id[UMI_EDITOR_SIGNATURE_PROVIDER_CAPACITY];
    char label[UMI_EDITOR_SIGNATURE_LABEL_CAPACITY];
    char documentation[UMI_EDITOR_SIGNATURE_DOCUMENTATION_CAPACITY];
    char return_type[UMI_EDITOR_SIGNATURE_RETURN_TYPE_CAPACITY];
    UmiEditorSourceLocation location;
    size_t parameter_count;
    int32_t score;
    int deprecated;
    int visible;
} UmiEditorSignatureHelpItem;

/**
 * Represent the editor signature help context data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSignatureHelpContext {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorSignatureHelpTriggerKind trigger_kind;
    uint32_t trigger_character;
    uint64_t document_revision;
    size_t active_parameter_ordinal;
    int has_active_parameter;
} UmiEditorSignatureHelpContext;

/**
 * Represent the editor signature help snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSignatureHelpSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t signature_count;
    size_t visible_signature_count;
    size_t deprecated_signature_count;
    size_t active_index;
    uint64_t revision;
    char active_signature_id[UMI_EDITOR_SIGNATURE_ID_CAPACITY];
    UmiEditorSignatureHelpContext context;
    int has_active_signature;
    int presented;
} UmiEditorSignatureHelpSnapshot;

/**
 * Represent the editor signature help model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSignatureHelpModel UmiEditorSignatureHelpModel;

/**
 * Initialise editor signature help model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_signature_help_model_create(
    UmiEditorSignatureHelpModel **out_model);
/**
 * Release or reset state held by editor signature help model so the same storage can be
 * reused safely.
 */
void umi_editor_signature_help_model_destroy(
    UmiEditorSignatureHelpModel *model);
/**
 * Release or reset state held by editor signature help model so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_signature_help_model_clear(
    UmiEditorSignatureHelpModel *model);
/**
 * Provide the editor signature help model upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_upsert(
    UmiEditorSignatureHelpModel *model,
    const UmiEditorSignatureHelpItem *item);
/**
 * Remove editor signature help model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_signature_help_model_remove(
    UmiEditorSignatureHelpModel *model,
    const char *signature_id);
/**
 * Provide the editor signature help model sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_sort(
    UmiEditorSignatureHelpModel *model);
/**
 * Provide the editor signature help model set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_set_context(
    UmiEditorSignatureHelpModel *model,
    const UmiEditorSignatureHelpContext *context);
/**
 * Provide the editor signature help model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_set_active(
    UmiEditorSignatureHelpModel *model,
    const char *signature_id);
/**
 * Provide the editor signature help model select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_select_next(
    UmiEditorSignatureHelpModel *model,
    int wrap);
/**
 * Provide the editor signature help model select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_signature_help_model_select_previous(
    UmiEditorSignatureHelpModel *model,
    int wrap);
/**
 * Provide the editor signature help model set presented operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_signature_help_model_set_presented(
    UmiEditorSignatureHelpModel *model,
    int presented);
/**
 * Find editor signature help model while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_signature_help_model_at(
    const UmiEditorSignatureHelpModel *model,
    size_t index,
    UmiEditorSignatureHelpItem *out_item);
/**
 * Provide the editor signature help model active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_active(
    const UmiEditorSignatureHelpModel *model,
    UmiEditorSignatureHelpItem *out_item);
/**
 * Provide the editor signature help model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_signature_help_model_snapshot(
    const UmiEditorSignatureHelpModel *model,
    UmiEditorSignatureHelpSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor signature help model without changing
 * their state.
 */
size_t umi_editor_signature_help_model_count(
    const UmiEditorSignatureHelpModel *model);
/**
 * Provide the editor signature help model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_signature_help_model_revision(
    const UmiEditorSignatureHelpModel *model);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SIGNATURE_HELP_MODEL_H */
