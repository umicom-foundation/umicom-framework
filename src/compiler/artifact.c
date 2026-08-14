/* Umicom Framework | Compiler artifacts | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/artifact.h"
#include <string.h>
UmiStatus umi_compiler_artifact_store_add(UmiCompilerArtifactStore *store,const UmiCompilerArtifact *artifact)
{
    size_t index;
    if (store == NULL || artifact == NULL || artifact->artifact_id[0] == '\0' || artifact->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].artifact_id,artifact->artifact_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_COMPILER_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *artifact; store->revision += 1U; return UMI_STATUS_OK;
}
const UmiCompilerArtifact *umi_compiler_artifact_store_find(const UmiCompilerArtifactStore *store,const char *artifact_id) { size_t index; if (store == NULL || artifact_id == NULL) return NULL; for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].artifact_id,artifact_id) == 0) return &store->items[index]; return NULL; }
size_t umi_compiler_artifact_store_count_kind(const UmiCompilerArtifactStore *store,UmiCompilerArtifactKind kind) { size_t index; size_t count = 0U; if (store == NULL) return 0U; for (index = 0U; index < store->count; ++index) if (store->items[index].kind == kind) count += 1U; return count; }
