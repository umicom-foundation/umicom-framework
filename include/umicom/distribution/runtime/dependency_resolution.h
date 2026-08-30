/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/dependency_resolution.h
 *
 * PURPOSE:
 *   bounded dependency graph with cycle detection and topological readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_RESOLUTION_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEPENDENCY_RESOLUTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrDependencyEdge { uint16_t from; uint16_t to; } UmiDrDependencyEdge;
typedef struct UmiDrDependencyResolution { size_t node_count; UmiDrDependencyEdge edges[UMI_DR_MAX_DEPENDENCIES]; size_t edge_count; } UmiDrDependencyResolution;
void umi_dr_dependency_resolution_init(UmiDrDependencyResolution *graph,size_t node_count);
UmiStatus umi_dr_dependency_resolution_add(UmiDrDependencyResolution *graph,size_t from,size_t to);
bool umi_dr_dependency_resolution_has_cycle(const UmiDrDependencyResolution *graph);

#ifdef __cplusplus
}
#endif
#endif
