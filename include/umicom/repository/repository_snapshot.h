/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_snapshot.h
 *
 * PURPOSE:
 *   Compose all read-only probe outputs into the existing maintenance summary plus inspection metadata.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_SNAPSHOT_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_SNAPSHOT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Collect one consistent repository snapshot from Framework-owned probes. */
UmiStatus umi_repository_snapshot_collect(const UmiRepositoryInspectionContext *context, UmiRepositoryInspectionSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
