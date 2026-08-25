/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/ignore_probe.h
 *
 * PURPOSE:
 *   Report whether the repository declares a root .gitignore policy file.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_IGNORE_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_IGNORE_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Check root .gitignore presence without reading or changing ignore policy. */
UmiStatus umi_repository_ignore_probe_read(const UmiRepositoryInspectionContext *context, int *out_present);

#ifdef __cplusplus
}
#endif
#endif
