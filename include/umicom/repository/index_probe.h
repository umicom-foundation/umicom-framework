/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/index_probe.h
 *
 * PURPOSE:
 *   Populate the existing index maintenance model from porcelain-v2 output.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INDEX_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_INDEX_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/index_status.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Run the read-only index probe and reuse the established maintenance state type. */
UmiStatus umi_repository_index_probe_read(const UmiRepositoryInspectionContext *context, UmiRepositoryIndexStatus *out_value);
#ifdef __cplusplus
}
#endif
#endif
