/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/tag_probe.h
 *
 * PURPOSE:
 *   Read an exact tag attached to HEAD when one exists.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_TAG_PROBE_H
#define INCLUDE_UMICOM_REPOSITORY_TAG_PROBE_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Run the read-only tag probe and copy its value into caller storage. */
UmiStatus umi_repository_tag_probe_read(const UmiRepositoryInspectionContext *context, char *out_value, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
