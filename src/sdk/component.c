/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/component.c
 *
 * PURPOSE:
 *   Describe one installed Framework SDK component and whether a consumer requires it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/component.h"
#include <stddef.h>
UmiStatus umi_sdk_component_validate(const UmiSdkComponent *c){if(c==NULL||c->component_id==NULL||c->component_id[0]=='\0'||c->target_name==NULL||c->target_name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
