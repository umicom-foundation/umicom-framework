/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/path.h
 *
 * PURPOSE:
 *   Validate and normalize repository-relative paths portably.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_CONTROL_PATH_H
#define UMICOM_REPOSITORY_CONTROL_PATH_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_repository_control_path_is_safe_relative(const char *path);
UmiStatus umi_repository_control_path_normalize(
    const char *path, char *out_path, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
