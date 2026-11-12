/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/artifact.h
 *
 * PURPOSE:
 *   Publish the public artifact contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_ARTIFACT_H
#define UMICOM_COMPILER_ARTIFACT_H
#include "umicom/compiler/common.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMPILER_MAX_ARTIFACTS 256U
/**
 * List the named compiler artifact kind values accepted by this public contract.
 */
typedef enum UmiCompilerArtifactKind { UMI_COMPILER_ARTIFACT_OBJECT = 1, UMI_COMPILER_ARTIFACT_EXECUTABLE, UMI_COMPILER_ARTIFACT_STATIC_LIBRARY, UMI_COMPILER_ARTIFACT_SHARED_LIBRARY, UMI_COMPILER_ARTIFACT_DEBUG_SYMBOLS, UMI_COMPILER_ARTIFACT_METADATA } UmiCompilerArtifactKind;
/**
 * Represent the compiler artifact data shared with callers of this public contract.
 */
typedef struct UmiCompilerArtifact { char artifact_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerArtifactKind kind; UmiCompilerLanguage language; uint64_t size_bytes; uint64_t created_at_ns; bool verified; } UmiCompilerArtifact;
/**
 * Represent the compiler artifact store data shared with callers of this public contract.
 */
typedef struct UmiCompilerArtifactStore { UmiCompilerArtifact items[UMI_COMPILER_MAX_ARTIFACTS]; size_t count; uint64_t revision; } UmiCompilerArtifactStore;
/**
 * Add compiler artifact store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_compiler_artifact_store_add(UmiCompilerArtifactStore *store,const UmiCompilerArtifact *artifact);
/**
 * Find compiler artifact store while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCompilerArtifact *umi_compiler_artifact_store_find(const UmiCompilerArtifactStore *store,const char *artifact_id);
/**
 * Provide the compiler artifact store count kind operation used by this module and its
 * client applications.
 */
size_t umi_compiler_artifact_store_count_kind(const UmiCompilerArtifactStore *store,UmiCompilerArtifactKind kind);

/** Admit bounded nonempty id/path, a named artifact kind and language UNKNOWN
 * through UAI. UNKNOWN supports language-neutral metadata. Validity does not
 * establish file existence, freshness, size or the truth of the verified flag. */
bool umi_compiler_artifact_valid(const UmiCompilerArtifact *artifact);
bool umi_compiler_artifact_store_valid(const UmiCompilerArtifactStore *store);
/** Add validates every active record before duplicate lookup. Duplicate ids retain
 * ALREADY_EXISTS even at capacity or maximum revision. New insertions at either
 * limit are refused. Every failure preserves the entire owner. Reads remain
 * available at maximum revision and return borrowed storage. Serialize access. */
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
