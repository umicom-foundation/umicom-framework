/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/toolchain.c
 *
 * PURPOSE:
 *   Record compiler and architecture evidence associated with an SDK installation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/toolchain.h"
#include <stddef.h>
UmiStatus umi_sdk_toolchain_validate(const UmiSdkToolchainEvidence *t){if(t==NULL||t->compiler_id==NULL||t->compiler_version==NULL||t->architecture==NULL||t->pointer_size==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
