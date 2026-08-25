/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/submodule_probe.h
 *
 * PURPOSE:
 *   Populate existing submodule maintenance counts from recursive submodule status.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_SUBMODULE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_SUBMODULE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/submodule_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Read recursive submodule status and preserve the existing maintenance summary type. */
UmiStatus umi_repository_submodule_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositorySubmoduleStatus *out_value);

#ifdef __cplusplus
}
#endif
#endif
