/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/version.c
 *
 * PURPOSE:
 *   Implement deterministic semantic-version comparison for ABI consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/version.h"
int umi_abi_version_compare(UmiVersion left,UmiVersion right){if(left.major!=right.major)return left.major<right.major?-1:1;if(left.minor!=right.minor)return left.minor<right.minor?-1:1;if(left.patch!=right.patch)return left.patch<right.patch?-1:1;return 0;}
int umi_abi_version_is_compatible(UmiVersion required,UmiVersion available){return required.major==available.major&&umi_abi_version_compare(required,available)<=0;}
