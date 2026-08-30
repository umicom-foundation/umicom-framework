/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_graph.h
 *
 * PURPOSE:
 *   Produce a simple dependency graph projection from the canonical submodule inventory.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_GRAPH_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_GRAPH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/repository/inventory.h"
/* Format root-to-submodule edges as deterministic text suitable for graph adapters. */
UmiStatus umi_repository_inspection_graph_format(const char *root_name, const UmiRepositoryInventory *inventory, char *out_text, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
