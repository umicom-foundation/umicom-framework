/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/gitmodules_probe.h
 *
 * PURPOSE:
 *   Bridge on-disk .gitmodules text into the existing canonical submodule inventory parser.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_GITMODULES_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_GITMODULES_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/repository/inventory.h"
/* Read .gitmodules and delegate parsing to the existing repository-control parser. */
UmiStatus umi_repository_gitmodules_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryInventory *out_inventory, int *out_present);

#ifdef __cplusplus
}
#endif
#endif
