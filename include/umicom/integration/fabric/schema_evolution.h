/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/schema_evolution.h
 *
 * PURPOSE:
 *   Represent allowed schema-version transitions and reject cyclic evolution graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SCHEMA_EVOLUTION_H
#define UMICOM_INTEGRATION_FABRIC_SCHEMA_EVOLUTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricSchemaEvolutionEdge { char from_id[UMI_FABRIC_ID_CAPACITY]; char to_id[UMI_FABRIC_ID_CAPACITY]; bool breaking; } UmiFabricSchemaEvolutionEdge;
typedef struct UmiFabricSchemaEvolution { UmiFabricSchemaEvolutionEdge edges[UMI_FABRIC_MAX_EDGES]; size_t count; } UmiFabricSchemaEvolution;
void umi_fabric_schema_evolution_init(UmiFabricSchemaEvolution *graph);
UmiStatus umi_fabric_schema_evolution_add(UmiFabricSchemaEvolution *graph,const char *from_id,const char *to_id,bool breaking);
bool umi_fabric_schema_evolution_has_cycle(const UmiFabricSchemaEvolution *graph);

#ifdef __cplusplus
}
#endif
#endif
