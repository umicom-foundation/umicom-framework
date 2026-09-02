/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/build_graph.c
 *
 * PURPOSE:
 *   Implement the build graph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler build graphs | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/build_graph.h"
#include <stdio.h>
#include <string.h>
/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiCompilerBuildGraph *graph,const char *id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || id == NULL) return SIZE_MAX; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(graph->steps[index].step_id,id) == 0) return index; return SIZE_MAX; }
/* Add compiler build graph only after its inputs and available capacity have been checked. */
UmiStatus umi_compiler_build_graph_add(UmiCompilerBuildGraph *graph,const UmiCompilerBuildStep *step) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || step == NULL || step->step_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (find_index(graph,step->step_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph->count >= UMI_COMPILER_MAX_BUILD_STEPS) return UMI_STATUS_CAPACITY_EXCEEDED; graph->steps[graph->count++] = *step; graph->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the compiler build graph add dependency operation used by this module and its
 * client applications.
 */
UmiStatus umi_compiler_build_graph_add_dependency(UmiCompilerBuildGraph *graph,const char *step_id,const char *dependency_id) { size_t step = find_index(graph,step_id); size_t dependency = find_index(graph,dependency_id); int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || step_id == NULL || dependency_id == NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (step == SIZE_MAX || dependency == SIZE_MAX) return UMI_STATUS_NOT_FOUND; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph->steps[step].dependency_count >= UMI_COMPILER_MAX_STEP_DEPENDENCIES) return UMI_STATUS_CAPACITY_EXCEEDED; length = snprintf(graph->steps[step].dependencies[graph->steps[step].dependency_count],sizeof(graph->steps[step].dependencies[0]),"%s",dependency_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (length < 0 || (size_t)length >= sizeof(graph->steps[step].dependencies[0])) return UMI_STATUS_CAPACITY_EXCEEDED; graph->steps[step].dependency_count += 1U; graph->revision += 1U; return UMI_STATUS_OK; }
/*
 * Find compiler build graph while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCompilerBuildStep *umi_compiler_build_graph_find(const UmiCompilerBuildGraph *graph,const char *step_id) { size_t index = find_index(graph,step_id); return index == SIZE_MAX ? NULL : &graph->steps[index]; }
/*
 * Provide the compiler build graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_build_graph_order(const UmiCompilerBuildGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count) { bool emitted[UMI_COMPILER_MAX_BUILD_STEPS] = {false}; size_t produced = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || out_indices == NULL || out_count == NULL || capacity < graph->count) return UMI_STATUS_INVALID_ARGUMENT; /* Continue only while work remains available; the loop body advances the state on each pass. */ while (produced < graph->count) { size_t index; bool progress = false; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (!emitted[index]) { size_t dependency; bool ready = true; /* Visit each bounded item once so every record receives the same rule. */ for (dependency = 0U; dependency < graph->steps[index].dependency_count; ++dependency) { size_t required = find_index(graph,graph->steps[index].dependencies[dependency]); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (required == SIZE_MAX) return UMI_STATUS_NOT_FOUND; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (!emitted[required]) ready = false; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if (ready) { out_indices[produced++] = index; emitted[index] = true; progress = true; } } /* Protect caller-owned memory by checking that required state is available before it is used. */ if (!progress) return UMI_STATUS_INVALID_STATE; } *out_count = produced; return UMI_STATUS_OK; }
