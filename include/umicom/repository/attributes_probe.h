/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/attributes_probe.h
 *
 * PURPOSE:
 *   Report whether the repository declares a root .gitattributes policy file.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_ATTRIBUTES_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_ATTRIBUTES_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Check root .gitattributes presence without changing attribute rules. */
UmiStatus umi_repository_attributes_probe_read(const UmiRepositoryInspectionContext *context, int *out_present);

#ifdef __cplusplus
}
#endif
#endif
