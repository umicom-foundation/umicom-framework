/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/import_organisation.h
 *
 * PURPOSE:
 *   Model import/include classification, deterministic ordering, duplicate
 *   detection and unused-import policy for multiple programming languages.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiEditorImportKind {
    UMI_EDITOR_IMPORT_SYSTEM = 1,
    UMI_EDITOR_IMPORT_THIRD_PARTY = 2,
    UMI_EDITOR_IMPORT_PROJECT = 3,
    UMI_EDITOR_IMPORT_RELATIVE = 4,
    UMI_EDITOR_IMPORT_MODULE = 5
} UmiEditorImportKind;

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

typedef struct UmiEditorImportOrganisationOptions {
    uint32_t struct_size;
    uint32_t api_version;
    int case_sensitive;
    int remove_unused;
    int remove_duplicates;
    int separate_groups;
} UmiEditorImportOrganisationOptions;

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

typedef struct UmiEditorImportOrganisationPlan
    UmiEditorImportOrganisationPlan;

UmiStatus umi_editor_import_organisation_plan_create(
    UmiEditorImportOrganisationPlan **out_plan);
void umi_editor_import_organisation_plan_destroy(
    UmiEditorImportOrganisationPlan *plan);
UmiStatus umi_editor_import_organisation_plan_clear(
    UmiEditorImportOrganisationPlan *plan);
UmiStatus umi_editor_import_organisation_plan_set_options(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportOrganisationOptions *options);
UmiStatus umi_editor_import_organisation_plan_upsert(
    UmiEditorImportOrganisationPlan *plan,
    const UmiEditorImportItem *item);
UmiStatus umi_editor_import_organisation_plan_remove(
    UmiEditorImportOrganisationPlan *plan,
    const char *item_id);
UmiStatus umi_editor_import_organisation_plan_analyse(
    UmiEditorImportOrganisationPlan *plan);
UmiStatus umi_editor_import_organisation_plan_sort(
    UmiEditorImportOrganisationPlan *plan);
UmiStatus umi_editor_import_organisation_plan_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t index,
    UmiEditorImportItem *out_item);
UmiStatus umi_editor_import_organisation_plan_kept_at(
    const UmiEditorImportOrganisationPlan *plan,
    size_t kept_index,
    UmiEditorImportItem *out_item);
UmiStatus umi_editor_import_organisation_plan_snapshot(
    const UmiEditorImportOrganisationPlan *plan,
    UmiEditorImportOrganisationSnapshot *out_snapshot);
size_t umi_editor_import_organisation_plan_count(
    const UmiEditorImportOrganisationPlan *plan);
uint64_t umi_editor_import_organisation_plan_revision(
    const UmiEditorImportOrganisationPlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_IMPORT_ORGANISATION_H */
