/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_interop_graph.c
 *
 * PURPOSE:
 *   Implement the test compiler interop graph behavior for
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
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiInteropGraph graph = {0}; UmiInteropNode c = {0},rust = {0}; UmiInteropEdge edge = {0}; UmiCompilerTarget target; size_t order[2],count = 0U; assert(umi_compiler_target_host(&target) == UMI_STATUS_OK); (void)strcpy(c.unit_id,"c-core"); c.language = UMI_COMPILER_LANGUAGE_C; assert(umi_compiler_abi_init(&c.abi,"stable-c",UMI_COMPILER_ABI_C,&target) == UMI_STATUS_OK); rust = c; (void)strcpy(rust.unit_id,"rust-ui"); rust.language = UMI_COMPILER_LANGUAGE_RUST; (void)strcpy(edge.producer_id,"c-core"); (void)strcpy(edge.consumer_id,"rust-ui"); edge.required = true; assert(umi_interop_graph_add_node(&graph,&c) == UMI_STATUS_OK); assert(umi_interop_graph_add_node(&graph,&rust) == UMI_STATUS_OK); assert(umi_interop_graph_add_edge(&graph,&edge) == UMI_STATUS_OK); assert(umi_interop_graph_order(&graph,order,2U,&count) == UMI_STATUS_OK); assert(count == 2U); return 0; }
