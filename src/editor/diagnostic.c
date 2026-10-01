/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/diagnostic.c
 *
 * PURPOSE:
 *   Implement editor diagnostics independently of their compiler, linter or language-server provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#include "umicom/editor/diagnostic.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiEditorDiagnosticSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiEditorDiagnosticSnapshot, document_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiEditorDiagnosticSnapshot, source, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiEditorDiagnosticSnapshot, code, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiEditorDiagnosticSnapshot, message, 0)
};
UmiStatus umi_editor_diagnostic_snapshot_validate(const UmiEditorDiagnosticSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}

#include <stdlib.h>
#include <string.h>
struct UmiEditorDiagnosticRegistry { UmiEditorDiagnosticSnapshot items[UMI_EDITOR_DIAGNOSTIC_CAPACITY]; size_t count; uint64_t revision; };
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorDiagnosticRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<r->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->items[i].id,id)==0)return i;return SIZE_MAX;}
/*
 * Initialise editor diagnostic registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_diagnostic_registry_create(UmiEditorDiagnosticRegistry **out){UmiEditorDiagnosticRegistry*r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;r=calloc(1U,sizeof(*r));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->revision=1U;*out=r;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor diagnostic registry so the same storage can be
 * reused safely.
 */
void umi_editor_diagnostic_registry_destroy(UmiEditorDiagnosticRegistry*r){free(r);}
/*
 * Provide the editor diagnostic registry upsert operation used by this module and its
 * client applications.
 */
/* The former unchecked compact upsert is retained for review. The public
 * replacement below adds Framework text-boundary and revision guards before
 * running its unchanged insertion/replacement logic. */
#if 0
UmiStatus umi_editor_diagnostic_registry_upsert(UmiEditorDiagnosticRegistry*r,const UmiEditorDiagnosticSnapshot*item){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_EDITOR_DIAGNOSTIC_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDiagnosticSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;}
#endif
UmiStatus umi_editor_diagnostic_registry_upsert(UmiEditorDiagnosticRegistry*r,const UmiEditorDiagnosticSnapshot*item){
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (r == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_editor_diagnostic_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (r->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||item==NULL||item->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,item->id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=UMI_EDITOR_DIAGNOSTIC_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;i=r->count++;}r->items[i]=*item;r->items[i].struct_size=(uint32_t)sizeof(UmiEditorDiagnosticSnapshot);r->items[i].api_version=1U;r->items[i].revision=++r->revision;return UMI_STATUS_OK;
}
/*
 * Remove editor diagnostic registry while keeping the remaining records in a valid and
 * discoverable state.
 */
/* The guarded mutation preserves monotonic observation tokens. The former
 * unchecked counter increment is retained here for engineering review. */
#if 0
UmiStatus umi_editor_diagnostic_registry_remove(UmiEditorDiagnosticRegistry*r,const char*id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
#endif
UmiStatus umi_editor_diagnostic_registry_remove(UmiEditorDiagnosticRegistry*r,const char*id){
    /* Refuse before removing or editing records when the registry cannot
     * issue a fresh observation token. A wrapped token could accept stale work. */
    if (r != NULL && r->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof(r->items[0]));r->count--;r->revision++;return UMI_STATUS_OK;}
/*
 * Find editor diagnostic registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diagnostic_registry_find(const UmiEditorDiagnosticRegistry*r,const char*id,UmiEditorDiagnosticSnapshot*out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_index(r,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Find editor diagnostic registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_diagnostic_registry_at(const UmiEditorDiagnosticRegistry*r,size_t i,UmiEditorDiagnosticSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i>=r->count)return UMI_STATUS_NOT_FOUND;*out=r->items[i];return UMI_STATUS_OK;}
/*
 * Return the number of records represented by editor diagnostic registry without changing
 * their state.
 */
size_t umi_editor_diagnostic_registry_count(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->count:0U;}
/*
 * Provide the editor diagnostic registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_diagnostic_registry_revision(const UmiEditorDiagnosticRegistry*r){return r!=NULL?r->revision:0U;}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_editor_diagnostic_registry_upsert_many,
    UmiEditorDiagnosticRegistry, UmiEditorDiagnosticSnapshot,
    umi_editor_diagnostic_snapshot_validate, umi_editor_diagnostic_registry_upsert, UMI_EDITOR_DIAGNOSTIC_CAPACITY)

/* Provider refreshes must replace this document as a unit, preserving other
 * documents and the last good list until all new records are accepted. */
UMI_DEFINE_SNAPSHOT_DOCUMENT_REPLACE(umi_editor_diagnostic_registry_replace_document,
    UmiEditorDiagnosticRegistry, UmiEditorDiagnosticSnapshot,
    umi_editor_diagnostic_snapshot_validate, umi_editor_diagnostic_registry_upsert, UMI_EDITOR_DIAGNOSTIC_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_editor_diagnostic_registry_capture,
    umi_editor_diagnostic_registry_replace_if_current, UmiEditorDiagnosticRegistry, UmiEditorDiagnosticSnapshot,
    umi_editor_diagnostic_snapshot_validate, umi_editor_diagnostic_registry_upsert, UMI_EDITOR_DIAGNOSTIC_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_editor_diagnostic_registry_edit_if_current,
    UmiEditorDiagnosticRegistry, UmiEditorDiagnosticSnapshot, UmiEditorDiagnosticEdit,
    umi_editor_diagnostic_snapshot_validate, umi_editor_diagnostic_registry_upsert, umi_editor_diagnostic_registry_remove, UMI_EDITOR_DIAGNOSTIC_CAPACITY)

/* Read accepted diagnostic records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_editor_diagnostic_registry_read_page,
    UmiEditorDiagnosticRegistry, UmiEditorDiagnosticSnapshot, UMI_EDITOR_DIAGNOSTIC_CAPACITY)
