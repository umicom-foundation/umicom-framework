/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/umicc_provider.h
 *
 * PURPOSE:
 *   Publish the public umicc provider contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_UMICC_PROVIDER_H
#define UMICOM_COMPILER_UMICC_PROVIDER_H
#include "umicom/compiler/provider.h"
/**
 * Provide the compiler umicc provider operation used by this module and its client
 * applications.
 */
UmiCompilerProvider umi_compiler_umicc_provider(const char *executable,const char *target);
#endif
