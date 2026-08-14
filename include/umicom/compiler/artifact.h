/* Umicom Framework | Compiler artifacts | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_ARTIFACT_H
#define UMICOM_COMPILER_ARTIFACT_H
#include "umicom/compiler/common.h"
#define UMI_COMPILER_MAX_ARTIFACTS 256U
typedef enum UmiCompilerArtifactKind { UMI_COMPILER_ARTIFACT_OBJECT = 1, UMI_COMPILER_ARTIFACT_EXECUTABLE, UMI_COMPILER_ARTIFACT_STATIC_LIBRARY, UMI_COMPILER_ARTIFACT_SHARED_LIBRARY, UMI_COMPILER_ARTIFACT_DEBUG_SYMBOLS, UMI_COMPILER_ARTIFACT_METADATA } UmiCompilerArtifactKind;
typedef struct UmiCompilerArtifact { char artifact_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerArtifactKind kind; UmiCompilerLanguage language; uint64_t size_bytes; uint64_t created_at_ns; bool verified; } UmiCompilerArtifact;
typedef struct UmiCompilerArtifactStore { UmiCompilerArtifact items[UMI_COMPILER_MAX_ARTIFACTS]; size_t count; uint64_t revision; } UmiCompilerArtifactStore;
UmiStatus umi_compiler_artifact_store_add(UmiCompilerArtifactStore *store,const UmiCompilerArtifact *artifact);
const UmiCompilerArtifact *umi_compiler_artifact_store_find(const UmiCompilerArtifactStore *store,const char *artifact_id);
size_t umi_compiler_artifact_store_count_kind(const UmiCompilerArtifactStore *store,UmiCompilerArtifactKind kind);
#endif
