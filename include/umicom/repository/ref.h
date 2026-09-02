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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_REF_H
#define UMICOM_REPOSITORY_REF_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Check that repository ref satisfies its contract before another service relies on it.
 */
int umi_repository_ref_is_valid(const char *text);
/**
 * Copy repository ref into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_repository_ref_copy(
    const char *text, char *out_ref, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
