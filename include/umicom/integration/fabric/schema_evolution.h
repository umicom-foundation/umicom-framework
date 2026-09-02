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


/**
 * Represent the fabric schema evolution edge data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricSchemaEvolutionEdge { char from_id[UMI_FABRIC_ID_CAPACITY]; char to_id[UMI_FABRIC_ID_CAPACITY]; bool breaking; } UmiFabricSchemaEvolutionEdge;
/**
 * Represent the fabric schema evolution data shared with callers of this public contract.
 */
typedef struct UmiFabricSchemaEvolution { UmiFabricSchemaEvolutionEdge edges[UMI_FABRIC_MAX_EDGES]; size_t count; } UmiFabricSchemaEvolution;
/**
 * Initialise fabric schema evolution from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_schema_evolution_init(UmiFabricSchemaEvolution *graph);
/**
 * Add fabric schema evolution only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_schema_evolution_add(UmiFabricSchemaEvolution *graph,const char *from_id,const char *to_id,bool breaking);
/**
 * Provide the fabric schema evolution has cycle operation used by this module and its
 * client applications.
 */
bool umi_fabric_schema_evolution_has_cycle(const UmiFabricSchemaEvolution *graph);

#ifdef __cplusplus
}
#endif
#endif
