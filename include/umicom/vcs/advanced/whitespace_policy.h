/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/whitespace_policy.h
 *
 * PURPOSE:
 *   Normalize text under explicit whitespace policy for reliable comparisons.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_WHITESPACE_POLICY_H
#define UMICOM_VCS_ADVANCED_WHITESPACE_POLICY_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_vcs_advanced_whitespace_normalize(const char *input,UmiVcsWhitespaceMode mode,
                                                 char *output,size_t capacity);
int umi_vcs_advanced_whitespace_equal(const char *left,const char *right,UmiVcsWhitespaceMode mode);
#ifdef __cplusplus
}
#endif
#endif
