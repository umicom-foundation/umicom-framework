/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/artifact.c
 *
 * PURPOSE:
 *   Implement the artifact behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/artifact.h"
#include <string.h>
#include "catalogue_validation.h"

/* Complete owner admission and explicit publication limits replace unchecked traversal. The original operations are retained for review of catalogue, cache replacement and invalidation behavior. The previous implementation is retained for review. */
#if 0
/*
 * Add compiler artifact store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_compiler_artifact_store_add(UmiCompilerArtifactStore *store,const UmiCompilerArtifact *artifact)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || artifact == NULL || artifact->artifact_id[0] == '\0' || artifact->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(store->items[index].artifact_id,artifact->artifact_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_COMPILER_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *artifact; store->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Find compiler artifact store while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCompilerArtifact *umi_compiler_artifact_store_find(const UmiCompilerArtifactStore *store,const char *artifact_id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store == NULL || artifact_id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < store->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(store->items[index].artifact_id,artifact_id) == 0) return &store->items[index]; return NULL; }
/*
 * Provide the compiler artifact store count kind operation used by this module and its
 * client applications.
 */
size_t umi_compiler_artifact_store_count_kind(const UmiCompilerArtifactStore *store,UmiCompilerArtifactKind kind) { size_t index; size_t count = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store == NULL) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < store->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store->items[index].kind == kind) count += 1U; return count; }
#endif
/* Every active record is admitted before lookup. A match near the start must not
 * hide a malformed later record. Metadata may describe a language-neutral file. */
bool umi_compiler_artifact_valid(const UmiCompilerArtifact *artifact)
{
    return artifact != NULL &&
        UmiCompilerCatalogueText(artifact->artifact_id,sizeof(artifact->artifact_id),true) &&
        UmiCompilerCatalogueText(artifact->path,sizeof(artifact->path),true) &&
        artifact->kind >= UMI_COMPILER_ARTIFACT_OBJECT && artifact->kind <= UMI_COMPILER_ARTIFACT_METADATA &&
        artifact->language >= UMI_COMPILER_LANGUAGE_UNKNOWN && artifact->language <= UMI_COMPILER_LANGUAGE_UAI;
}
bool umi_compiler_artifact_store_valid(const UmiCompilerArtifactStore *store)
{
    if (store == NULL || store->count > UMI_COMPILER_MAX_ARTIFACTS) return false;
    for (size_t index = 0U; index < store->count; ++index)
        if (!umi_compiler_artifact_valid(&store->items[index])) return false;
    return true;
}
UmiStatus umi_compiler_artifact_store_add(UmiCompilerArtifactStore *store,const UmiCompilerArtifact *artifact)
{
    if (store == NULL || !umi_compiler_artifact_valid(artifact)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_compiler_artifact_store_valid(store)) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < store->count; ++index)
        if (strcmp(store->items[index].artifact_id,artifact->artifact_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count == UMI_COMPILER_MAX_ARTIFACTS || store->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* memmove also admits an incoming value composed in this unused slot. */
    (void)memmove(&store->items[store->count],artifact,sizeof(*artifact));
    store->count += 1U;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiCompilerArtifact *umi_compiler_artifact_store_find(const UmiCompilerArtifactStore *store,const char *artifact_id)
{
    if (!UmiCompilerCatalogueText(artifact_id,UMI_COMPILER_ID_CAPACITY,true) ||
        !umi_compiler_artifact_store_valid(store)) return NULL;
    for (size_t index = 0U; index < store->count; ++index)
        if (strcmp(store->items[index].artifact_id,artifact_id) == 0) return &store->items[index];
    return NULL;
}
size_t umi_compiler_artifact_store_count_kind(const UmiCompilerArtifactStore *store,UmiCompilerArtifactKind kind)
{
    size_t count = 0U;
    if (kind < UMI_COMPILER_ARTIFACT_OBJECT || kind > UMI_COMPILER_ARTIFACT_METADATA ||
        !umi_compiler_artifact_store_valid(store)) return 0U;
    for (size_t index = 0U; index < store->count; ++index)
        if (store->items[index].kind == kind) count += 1U;
    return count;
}
