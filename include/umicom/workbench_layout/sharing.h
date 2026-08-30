/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/sharing.h
 *
 * PURPOSE:
 *   Create portable layout bundles with provenance, integrity evidence and explicit ownership transfer for import, export and source control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_SHARING_H
#define UMICOM_WORKBENCH_LAYOUT_SHARING_H

#include "umicom/workbench_layout/json.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutShareManifest {
    uint32_t structure_size;
    char bundle_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_name[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char source_owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char source_owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char source_workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char exported_by[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t exported_at_ms;
    uint64_t layout_revision;
    uint64_t content_hash;
    uint32_t schema_version;
    bool includes_runtime_geometry;
    bool includes_audit;
} UmiWorkbenchLayoutShareManifest;

typedef struct UmiWorkbenchLayoutShareBundle {
    uint32_t structure_size;
    UmiWorkbenchLayoutShareManifest manifest;
    UmiWorkbenchLayoutDocument document;
    char signature[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    bool signature_present;
} UmiWorkbenchLayoutShareBundle;

typedef struct UmiWorkbenchLayoutImportOptions {
    uint32_t structure_size;
    char target_layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char target_owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char target_owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char target_workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    bool preserve_source_audit;
    bool preserve_runtime_geometry;
    bool mark_shared;
    bool require_integrity_match;
} UmiWorkbenchLayoutImportOptions;

UmiWorkbenchLayoutImportOptions
umi_workbench_layout_import_options_default(void);

UmiStatus umi_workbench_layout_share_bundle_create(
    const UmiWorkbenchLayoutDocument *document,
    const char *bundle_id,
    const char *exported_by,
    uint64_t exported_at_ms,
    bool include_runtime_geometry,
    bool include_audit,
    UmiWorkbenchLayoutShareBundle *out_bundle);

UmiStatus umi_workbench_layout_share_bundle_validate(
    const UmiWorkbenchLayoutShareBundle *bundle);

UmiStatus umi_workbench_layout_share_bundle_import(
    const UmiWorkbenchLayoutShareBundle *bundle,
    const UmiWorkbenchLayoutImportOptions *options,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_share_bundle_encode(
    const UmiWorkbenchLayoutShareBundle *bundle,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_share_bundle_decode(
    const char *text,
    size_t length,
    UmiWorkbenchLayoutShareBundle *out_bundle);

#ifdef __cplusplus
}
#endif

#endif
