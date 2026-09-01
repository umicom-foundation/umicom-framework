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
UmiStatus umi_sdk_target_validate(const UmiSdkTarget *t){if(t==NULL||t->target_name==NULL||t->target_name[0]=='\0'||t->component_id==NULL||t->component_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
