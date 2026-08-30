/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/nested_repository_probe.h
 *
 * PURPOSE:
 *   Report whether the inspected repository is itself nested inside a Git superproject.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_NESTED_REPOSITORY_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_NESTED_REPOSITORY_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Read Git superproject metadata without walking unrelated parent directories. */
UmiStatus umi_repository_nested_repository_probe_read(const UmiRepositoryInspectionContext *context, int *out_nested);

#ifdef __cplusplus
}
#endif
#endif
