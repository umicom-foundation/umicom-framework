/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/status_probe.h
 *
 * PURPOSE:
 *   Collect one porcelain-v2 status snapshot that branch, worktree and index projections can share.
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

#ifndef UMICOM_REPOSITORY_STATUS_PROBE_H
#define UMICOM_REPOSITORY_STATUS_PROBE_H

#include "umicom/repository/porcelain_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Execute Git status once and parse all branch, worktree and index evidence. */
UmiStatus umi_repository_status_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryPorcelainStatus *out_status);

#ifdef __cplusplus
}
#endif
#endif
