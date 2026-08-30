/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inspection_text.h
 *
 * PURPOSE:
 *   Format inspection evidence for CLI or UI text surfaces without embedding presentation logic in probes.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_INSPECTION_TEXT_H
#define INCLUDE_UMICOM_REPOSITORY_INSPECTION_TEXT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/repository/inspection_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/repository/inspection_service.h"
/* Format a stable human-readable inspection summary into caller-owned storage. */
UmiStatus umi_repository_inspection_text_format(const UmiRepositoryInspectionService *service, char *out_text, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
