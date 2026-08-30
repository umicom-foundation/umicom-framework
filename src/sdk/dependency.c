/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/dependency.c
 *
 * PURPOSE:
 *   Describe one external dependency required by an exported Framework target.
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
#include "umicom/sdk/dependency.h"
#include <stddef.h>
UmiStatus umi_sdk_dependency_validate(const UmiSdkDependency *d){if(d==NULL||d->package_name==NULL||d->package_name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
