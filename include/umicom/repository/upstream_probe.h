/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/upstream_probe.h
 *
 * PURPOSE:
 *   Read the configured upstream ref for the current branch.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_UPSTREAM_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_UPSTREAM_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Run the read-only upstream probe and copy its value into caller storage. */
UmiStatus umi_repository_upstream_probe_read(const UmiRepositoryInspectionContext *context, char *out_value, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
