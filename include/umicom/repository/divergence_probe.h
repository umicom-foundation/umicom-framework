/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/divergence_probe.h
 *
 * PURPOSE:
 *   Read ahead/behind counts relative to the configured upstream without network access.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_DIVERGENCE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_DIVERGENCE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryDivergence { size_t ahead; size_t behind; int has_upstream; } UmiRepositoryDivergence;
/* Read local commit divergence against @{u}; the probe never fetches or mutates remotes. */
UmiStatus umi_repository_divergence_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryDivergence *out_value);

#ifdef __cplusplus
}
#endif
#endif
