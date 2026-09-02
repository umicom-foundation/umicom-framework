/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/platform.c
 *
 * PURPOSE:
 *   Detect and compare the current operating-system and architecture boundary.
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
#include "umicom/abi/platform.h"
#include <string.h>
/*
 * Provide the abi platform current operation used by this module and its client
 * applications.
 */
UmiAbiPlatform umi_abi_platform_current(void){UmiAbiPlatform p;
#if defined(_WIN32)
p.os="windows";
#elif defined(__APPLE__)
p.os="macos";
#else
p.os="linux";
#endif
#if defined(__x86_64__)||defined(_M_X64)
p.architecture="x86_64";
#elif defined(__aarch64__)||defined(_M_ARM64)
p.architecture="aarch64";
#else
p.architecture="unknown";
#endif
p.pointer_size=sizeof(void*);{const unsigned short v=1U;p.little_endian=*((const unsigned char*)&v)==1U;}return p;}
/* Check that abi platform satisfies its contract before another service relies on it. */
UmiStatus umi_abi_platform_validate(const UmiAbiPlatform *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->os==NULL||p->architecture==NULL||p->pointer_size==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the abi platform compatible operation used by this module and its client
 * applications.
 */
int umi_abi_platform_compatible(const UmiAbiPlatform *a,const UmiAbiPlatform *b){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_abi_platform_validate(a)!=UMI_STATUS_OK||umi_abi_platform_validate(b)!=UMI_STATUS_OK)return 0;return strcmp(a->os,b->os)==0&&strcmp(a->architecture,b->architecture)==0&&a->pointer_size==b->pointer_size&&a->little_endian==b->little_endian;}
