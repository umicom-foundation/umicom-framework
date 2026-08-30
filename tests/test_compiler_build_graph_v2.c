/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_build_graph_v2.c
 *
 * PURPOSE:
 *   Implement the test compiler build graph v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void) { UmiCompilerBuildGraph *graph = calloc(1U,sizeof(*graph)); UmiCompilerBuildStep compile = {0},link = {0}; size_t order[2],count = 0U; assert(graph != NULL); (void)strcpy(compile.step_id,"compile-main"); compile.kind = UMI_COMPILER_BUILD_COMPILE; (void)strcpy(link.step_id,"link-app"); link.kind = UMI_COMPILER_BUILD_LINK; assert(umi_compiler_build_graph_add(graph,&compile) == UMI_STATUS_OK); assert(umi_compiler_build_graph_add(graph,&link) == UMI_STATUS_OK); assert(umi_compiler_build_graph_add_dependency(graph,"link-app","compile-main") == UMI_STATUS_OK); assert(umi_compiler_build_graph_order(graph,order,2U,&count) == UMI_STATUS_OK); assert(count == 2U); assert(order[0] == 0U); free(graph); return 0; }
