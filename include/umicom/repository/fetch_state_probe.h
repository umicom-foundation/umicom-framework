/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/fetch_state_probe.h
 *
 * PURPOSE:
 *   Report whether the configured upstream ref is available locally without performing a fetch.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_FETCH_STATE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_FETCH_STATE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Check local upstream ref resolution; this intentionally performs no network operation. */
UmiStatus umi_repository_fetch_state_probe_read(const UmiRepositoryInspectionContext *context, int *out_available);

#ifdef __cplusplus
}
#endif
#endif
