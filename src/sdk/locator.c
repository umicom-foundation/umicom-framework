/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/locator.c
 *
 * PURPOSE:
 *   Locate an installed SDK from an explicit prefix or conventional directory.
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
#include "umicom/sdk/locator.h"
#include "umicom/sdk/package.h"
#include "umicom/platform/filesystem.h"
#include "umicom/base/version.h"
#include <stdio.h>
static int valid(const char *p){UmiSdkPackage pkg;return umi_sdk_package_from_prefix(p,&pkg)==UMI_STATUS_OK&&umi_fs_is_file(pkg.config_file);}
UmiStatus umi_sdk_locate(const char *preferred,char *out,size_t cap){const char *candidates[]={preferred,"C:/Dev/umicom/sdk/umicom-framework-" UMICOM_FRAMEWORK_VERSION_STRING "-local","/usr/local","/usr"};size_t i;int n;if(out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<sizeof(candidates)/sizeof(candidates[0]);++i){if(candidates[i]!=NULL&&valid(candidates[i])){n=snprintf(out,cap,"%s",candidates[i]);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
