/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/sha.h
 *
 * PURPOSE:
 *   Validate and normalize Git object identifiers without shell parsing.
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
#ifndef UMICOM_REPOSITORY_SHA_H
#define UMICOM_REPOSITORY_SHA_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_repository_sha_is_valid(const char *text);
UmiStatus umi_repository_sha_normalize(
    const char *text, char *out_sha, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
