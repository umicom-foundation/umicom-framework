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

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/dependency.h"
#include <stddef.h>
/* Check that sdk dependency satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_dependency_validate(const UmiSdkDependency *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->package_name==NULL||d->package_name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
