/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/dynamic_library_semantics.c
 *
 * PURPOSE:
 *   Describe shared-library naming and dynamic-loading support for plug-in portability.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/dynamic_library_semantics.h"

#include <stdio.h>
UmiCtDynamicLibrarySemantics umi_ct_dynamic_library_semantics_default(UmiCtOperatingSystem os){UmiCtDynamicLibrarySemantics s={{0},{0},true,true,true};if(os==UMI_CT_OS_WINDOWS){(void)umi_ct_copy(s.suffix,sizeof(s.suffix),".dll");}else if(os==UMI_CT_OS_MACOS){(void)umi_ct_copy(s.prefix,sizeof(s.prefix),"lib");(void)umi_ct_copy(s.suffix,sizeof(s.suffix),".dylib");}else if(os==UMI_CT_OS_BARE_METAL){s.dynamic_loading=false;s.global_symbols=false;s.unload=false;}else{(void)umi_ct_copy(s.prefix,sizeof(s.prefix),"lib");(void)umi_ct_copy(s.suffix,sizeof(s.suffix),".so");}return s;}
UmiStatus umi_ct_dynamic_library_name(const UmiCtDynamicLibrarySemantics*s,const char*b,char*out,size_t c){int n;if(s==NULL||b==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,c,"%s%s%s",s->prefix,b,s->suffix);return n<0||(size_t)n>=c?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
