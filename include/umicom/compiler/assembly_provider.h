/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/assembly_provider.h
 *
 * PURPOSE:
 *   Publish the public assembly provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_ASSEMBLY_PROVIDER_H
#define UMICOM_COMPILER_ASSEMBLY_PROVIDER_H
#include "umicom/compiler/provider.h"
/**
 * Provide the compiler assembly provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_assembly_provider(const char *executable,const char *target);
#endif
