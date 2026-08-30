/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/probe.c
 *
 * PURPOSE:
 *   Probe required public headers, libraries and CMake package files.
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
#include "umicom/sdk/probe.h"
#include "umicom/sdk/package.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
UmiStatus umi_sdk_probe_prefix(const char *p,UmiSdkProbeResult *o){UmiSdkPackage pkg;char header[1024];char libdir[1024];if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_sdk_package_from_prefix(p,&pkg)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;(void)snprintf(header,sizeof(header),"%s/include/umicom/umicom.h",p);(void)snprintf(libdir,sizeof(libdir),"%s/lib",p);o->package_config=umi_fs_is_file(pkg.config_file)&&umi_fs_is_file(pkg.targets_file);o->public_header=umi_fs_is_file(header);o->libraries=umi_fs_is_directory(libdir);o->passed=o->package_config&&o->public_header&&o->libraries;return UMI_STATUS_OK;}
