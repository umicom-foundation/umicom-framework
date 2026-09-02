/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/toolchain.c
 *
 * PURPOSE:
 *   Record compiler and architecture evidence associated with an SDK installation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/toolchain.h"
#include <stddef.h>
/* Check that sdk toolchain satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||t->compiler_id==NULL||t->compiler_version==NULL||t->architecture==NULL||t->pointer_size==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
