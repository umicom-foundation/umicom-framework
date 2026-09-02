/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/import_organisation.h
 *
 * PURPOSE:
 *   Model import/include classification, deterministic ordering, duplicate
 *   detection and unused-import policy for multiple programming languages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_IMPORT_ORGANISATION_H
#define UMICOM_EDITOR_IMPORT_ORGANISATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION 1U
#define UMI_EDITOR_IMPORT_ID_CAPACITY 128U
#define UMI_EDITOR_IMPORT_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_IMPORT_SOURCE_CAPACITY 512U
#define UMI_EDITOR_IMPORT_SYMBOL_CAPACITY 256U
#define UMI_EDITOR_IMPORT_TEXT_CAPACITY 1024U

/**
 * List the named editor import kind values accepted by this public contract.
 */
typedef enum UmiEditorImportKind {
    UMI_EDITOR_IMPORT_SYSTEM = 1,
    UMI_EDITOR_IMPORT_THIRD_PARTY = 2,
    UMI_EDITOR_IMPORT_PROJECT = 3,
    UMI_EDITOR_IMPORT_RELATIVE = 4,
    UMI_EDITOR_IMPORT_MODULE = 5
} UmiEditorImportKind;

/**
 * Represent the editor import item data shared with callers of this public contract.
 */
typedef struct UmiEditorImportItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_IMPORT_ID_CAPACITY];
    char provider_id[UMI_EDITOR_IMPORT_PROVIDER_CAPACITY];
    char canonical_source[UMI_EDITOR_IMPORT_SOURCE_CAPACITY];
    char imported_symbol[UMI_EDITOR_IMPORT_SYMBOL_CAPACITY];
    char original_text[UMI_EDITOR_IMPORT_TEXT_CAPACITY];
    UmiEditorSourceLocation location;
    UmiEditorImportKind kind;
    int32_t group_order;
    int used;
    int pinned;
    int duplicate;
    int removable;
} UmiEditorImportItem;

/**
 * Represent the editor import organisation options data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorImportOrganisationOptions {
    uint32_t struct_size;
    uint32_t api_version;
    int case_sensitive;
    int remove_unused;
    int remove_duplicates;
    int separate_groups;
} UmiEditorImportOrganisationOptions;

/**
 * Represent the editor import organisation snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorImportOrganisationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t import_count;
    size_t group_count;
    size_t duplicate_count;
    size_t unused_count;
    size_t removable_count;
    uint64_t revision;
    int analysed;
} UmiEditorImportOrganisationSnapshot;

/**
 * Represent the editor import organisation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorImportOrganisationPlan
    UmiEditorImportOrganisationPlan;

/**
 * Initialise editor import organisation plan from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_import_organisation_plan_create(
    UmiEditorImportOrganisationPlan **out_plan);
/**
 * Release or reset state held by editor import organisation plan so the same storage can
 * be reused safely.
 */
void umi_editor_import_organisation_plan_destroy(
    UmiEditorImportOrganisationPlan *plan);
/**
 * Release or reset state held by editor import organisation plan so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_import_organisation_plan_clear(
    UmiEditorImportOrganisationPlan *plan);
/**
 * Provide the editor import organisation plan set options operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_import_organisation_plan_set_options(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportOrganisationOptions *options);
/**
 * Provide the editor import organisation plan upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_import_organisation_plan_upsert(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportItem *item);
/**
 * Remove editor import organisation plan while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_editor_import_organisation_plan_remove(
    UmiEditorImportOrganisationPlan *plan,
    const char *item_id);
/**
 * Provide the editor import organisation plan analyse operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_import_organisation_plan_analyse(
    UmiEditorImportOrganisationPlan *plan);
/**
 * Provide the editor import organisation plan sort operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_import_organisation_plan_sort(
    UmiEditorImportOrganisationPlan *plan);
/**
 * Find editor import organisation plan while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_import_organisation_plan_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t index,
    UmiEditorImportItem *out_item);
/**
 * Find editor import organisation plan kept while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_import_organisation_plan_kept_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t kept_index,
    UmiEditorImportItem *out_item);
/**
 * Provide the editor import organisation plan snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_import_organisation_plan_snapshot(
    const UmiEditorImportOrganisationPlan *plan,
    UmiEditorImportOrganisationSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor import organisation plan without
 * changing their state.
 */
size_t umi_editor_import_organisation_plan_count(
    const UmiEditorImportOrganisationPlan *plan);
/**
 * Provide the editor import organisation plan revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_import_organisation_plan_revision(
    const UmiEditorImportOrganisationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_IMPORT_ORGANISATION_H */
