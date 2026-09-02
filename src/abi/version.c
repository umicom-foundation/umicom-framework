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

/*
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/version.h"
/*
 * Provide the abi version compare operation used by this module and its client
 * applications.
 */
int umi_abi_version_compare(UmiVersion left,UmiVersion right){/* Apply this branch only when its contract condition is satisfied. */ if(left.major!=right.major)return left.major<right.major?-1:1;/* Apply this branch only when its contract condition is satisfied. */ if(left.minor!=right.minor)return left.minor<right.minor?-1:1;/* Apply this branch only when its contract condition is satisfied. */ if(left.patch!=right.patch)return left.patch<right.patch?-1:1;return 0;}
/*
 * Provide the abi version is compatible operation used by this module and its client
 * applications.
 */
int umi_abi_version_is_compatible(UmiVersion required,UmiVersion available){return required.major==available.major&&umi_abi_version_compare(required,available)<=0;}
