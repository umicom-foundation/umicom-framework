/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remote_probe.h
 *
 * PURPOSE:
 *   Populate existing remote maintenance state from local Git remote configuration.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REMOTE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_REMOTE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/remote_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Read local remote names and origin URL availability into the established remote model. */
UmiStatus umi_repository_remote_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryRemoteStatus *out_value);

#ifdef __cplusplus
}
#endif
#endif
