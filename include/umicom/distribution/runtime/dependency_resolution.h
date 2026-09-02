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

/**
 * Represent the dr dependency edge data shared with callers of this public contract.
 */
typedef struct UmiDrDependencyEdge { uint16_t from; uint16_t to; } UmiDrDependencyEdge;
/**
 * Represent the dr dependency resolution data shared with callers of this public contract.
 */
typedef struct UmiDrDependencyResolution { size_t node_count; UmiDrDependencyEdge edges[UMI_DR_MAX_DEPENDENCIES]; size_t edge_count; } UmiDrDependencyResolution;
/**
 * Initialise dr dependency resolution from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_dependency_resolution_init(UmiDrDependencyResolution *graph,size_t node_count);
/**
 * Add dr dependency resolution only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_dr_dependency_resolution_add(UmiDrDependencyResolution *graph,size_t from,size_t to);
/**
 * Provide the dr dependency resolution has cycle operation used by this module and its
 * client applications.
 */
bool umi_dr_dependency_resolution_has_cycle(const UmiDrDependencyResolution *graph);

#ifdef __cplusplus
}
#endif
#endif
