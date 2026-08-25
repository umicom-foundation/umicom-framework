/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/branch_probe.h
 *
 * PURPOSE:
 *   Populate the existing branch maintenance model from porcelain-v2 output.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_REPOSITORY_BRANCH_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_BRANCH_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/branch_status.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Run the read-only branch probe and reuse the established maintenance state type. */
UmiStatus umi_repository_branch_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryBranchStatus *out_value);
#ifdef __cplusplus
}
#endif
#endif
