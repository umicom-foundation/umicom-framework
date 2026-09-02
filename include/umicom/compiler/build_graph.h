/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/build_graph.h
 *
 * PURPOSE:
 *   Publish the public build graph contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_BUILD_GRAPH_H
#define UMICOM_COMPILER_BUILD_GRAPH_H
#include "umicom/compiler/plan.h"
#define UMI_COMPILER_MAX_BUILD_STEPS 192U
#define UMI_COMPILER_MAX_STEP_DEPENDENCIES 16U
/**
 * List the named compiler build step kind values accepted by this public contract.
 */
typedef enum UmiCompilerBuildStepKind { UMI_COMPILER_BUILD_COMPILE = 1, UMI_COMPILER_BUILD_ASSEMBLE, UMI_COMPILER_BUILD_LINK, UMI_COMPILER_BUILD_ARCHIVE, UMI_COMPILER_BUILD_CHECK } UmiCompilerBuildStepKind;
/**
 * Represent the compiler build step data shared with callers of this public contract.
 */
typedef struct UmiCompilerBuildStep { char step_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerBuildStepKind kind; UmiCompilerPlan plan; char dependencies[UMI_COMPILER_MAX_STEP_DEPENDENCIES][UMI_COMPILER_ID_CAPACITY]; size_t dependency_count; bool optional; } UmiCompilerBuildStep;
/**
 * Represent the compiler build graph data shared with callers of this public contract.
 */
typedef struct UmiCompilerBuildGraph { UmiCompilerBuildStep steps[UMI_COMPILER_MAX_BUILD_STEPS]; size_t count; uint64_t revision; } UmiCompilerBuildGraph;
/**
 * Add compiler build graph only after its inputs and available capacity have been checked.
 */
UmiStatus umi_compiler_build_graph_add(UmiCompilerBuildGraph *graph,const UmiCompilerBuildStep *step);
/**
 * Provide the compiler build graph add dependency operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_build_graph_add_dependency(UmiCompilerBuildGraph *graph,const char *step_id,const char *dependency_id);
/**
 * Find compiler build graph while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerBuildStep *umi_compiler_build_graph_find(const UmiCompilerBuildGraph *graph,const char *step_id);
/**
 * Provide the compiler build graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_build_graph_order(const UmiCompilerBuildGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count);
#endif
