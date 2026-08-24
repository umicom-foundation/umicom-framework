/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/ref.h
 *
 * PURPOSE:
 *   Validate Git reference names before they reach native process invocation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_REF_H
#define UMICOM_REPOSITORY_REF_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_repository_ref_is_valid(const char *text);
UmiStatus umi_repository_ref_copy(
    const char *text, char *out_ref, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
