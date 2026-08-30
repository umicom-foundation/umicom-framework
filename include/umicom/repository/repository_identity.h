/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/repository_identity.h
 *
 * PURPOSE:
 *   Read canonical repository root and worktree/bare identity.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_REPOSITORY_IDENTITY_H
#define INCLUDE_UMICOM_REPOSITORY_REPOSITORY_IDENTITY_H
#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Run the read-only repository identity and copy its value into caller storage. */
UmiStatus umi_repository_repository_identity_read(const UmiRepositoryInspectionContext *context, char *out_value, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
