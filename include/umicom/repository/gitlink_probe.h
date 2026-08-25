/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/gitlink_probe.h
 *
 * PURPOSE:
 *   Count index gitlinks by delegating record parsing to the established gitlink parser.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_GITLINK_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_GITLINK_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Count stage-0 mode-160000 records without reimplementing gitlink parsing. */
UmiStatus umi_repository_gitlink_probe_read(const UmiRepositoryInspectionContext *context, size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
