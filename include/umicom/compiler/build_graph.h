/* Umicom Framework | Native compiler build graph | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_BUILD_GRAPH_H
#define UMICOM_COMPILER_BUILD_GRAPH_H
#include "umicom/compiler/plan.h"
#define UMI_COMPILER_MAX_BUILD_STEPS 192U
#define UMI_COMPILER_MAX_STEP_DEPENDENCIES 16U
typedef enum UmiCompilerBuildStepKind { UMI_COMPILER_BUILD_COMPILE = 1, UMI_COMPILER_BUILD_ASSEMBLE, UMI_COMPILER_BUILD_LINK, UMI_COMPILER_BUILD_ARCHIVE, UMI_COMPILER_BUILD_CHECK } UmiCompilerBuildStepKind;
typedef struct UmiCompilerBuildStep { char step_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerBuildStepKind kind; UmiCompilerPlan plan; char dependencies[UMI_COMPILER_MAX_STEP_DEPENDENCIES][UMI_COMPILER_ID_CAPACITY]; size_t dependency_count; bool optional; } UmiCompilerBuildStep;
typedef struct UmiCompilerBuildGraph { UmiCompilerBuildStep steps[UMI_COMPILER_MAX_BUILD_STEPS]; size_t count; uint64_t revision; } UmiCompilerBuildGraph;
UmiStatus umi_compiler_build_graph_add(UmiCompilerBuildGraph *graph,const UmiCompilerBuildStep *step);
UmiStatus umi_compiler_build_graph_add_dependency(UmiCompilerBuildGraph *graph,const char *step_id,const char *dependency_id);
const UmiCompilerBuildStep *umi_compiler_build_graph_find(const UmiCompilerBuildGraph *graph,const char *step_id);
UmiStatus umi_compiler_build_graph_order(const UmiCompilerBuildGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count);
#endif
