/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/target.c
 *
 * PURPOSE:
 *   Describe one exported CMake target expected by SDK consumers.
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
#include "umicom/sdk/target.h"
#include <stddef.h>
/* Check that sdk target satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_target_validate(const UmiSdkTarget *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||t->target_name==NULL||t->target_name[0]=='\0'||t->component_id==NULL||t->component_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
