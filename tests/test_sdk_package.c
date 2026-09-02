/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_sdk_package.c
 *
 * PURPOSE:
 *   Validate sdk package behaviour for the major release hardening train.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused test protects one public contract so future Framework changes cannot silently break SDK or product consumers.
 */
#include <assert.h>
#include <string.h>
#include "umicom/sdk/sdk.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiSdkPackage p;assert(umi_sdk_package_from_prefix("/tmp/sdk",&p)==UMI_STATUS_OK);assert(strstr(p.config_file,"UmicomFrameworkConfig.cmake")!=NULL);return 0;}
