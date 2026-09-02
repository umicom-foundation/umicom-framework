/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/query.c
 *
 * PURPOSE:
 *   Initialise reusable context-host query requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/query.h"
#include <string.h>
/*
 * Initialise workbench context host query from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_host_query_init(UmiWorkbenchContextHostQuery *query,UmiWorkbenchContextHostQueryKind kind)
{/* Apply this branch only when its contract condition is satisfied. */ if(query){memset(query,0,sizeof(*query));query->structure_size=(uint32_t)sizeof(*query);query->kind=kind;}}
