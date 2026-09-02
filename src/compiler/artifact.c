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
/* Umicom Framework | Compiler artifacts | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/artifact.h"
#include <string.h>
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
