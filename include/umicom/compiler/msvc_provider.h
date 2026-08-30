/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/msvc_provider.h
 *
 * PURPOSE:
 *   Publish the public msvc provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_MSVC_PROVIDER_H
#define UMICOM_COMPILER_MSVC_PROVIDER_H
#include "umicom/compiler/provider.h"
UmiCompilerProvider umi_compiler_msvc_provider(const char *executable,const char *target);
#endif
