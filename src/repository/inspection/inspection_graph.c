/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_graph.c
 *
 * PURPOSE:
 *   Implement produce a simple dependency graph projection from the canonical submodule inventory.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/inspection_graph.h"

#include <stdio.h>
#include <string.h>
/* Build graph edges from canonical inventory records; no second dependency model is introduced. */
UmiStatus umi_repository_inspection_graph_format(const char *root_name, const UmiRepositoryInventory *inventory, char *out_text, size_t capacity)
{
    size_t index, used=0U; if(root_name==NULL||inventory==NULL||out_text==NULL||capacity==0U) return UMI_STATUS_INVALID_ARGUMENT; out_text[0]='\0';
    for(index=0U;index<inventory->count;++index){int written=snprintf(out_text+used,capacity-used,"%s -> %s\n",root_name,inventory->items[index].path); if(written<0) return UMI_STATUS_IO_ERROR; if((size_t)written>=capacity-used) return UMI_STATUS_CAPACITY_EXCEEDED; used+=(size_t)written;} return UMI_STATUS_OK;
}

