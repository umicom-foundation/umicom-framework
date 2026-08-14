/* Umicom Framework private compiler provider defaults | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_PROVIDER_DEFAULTS_PRIVATE_H
#define UMICOM_COMPILER_PROVIDER_DEFAULTS_PRIVATE_H
#include "umicom/compiler/provider.h"
UmiCompilerProvider umi_compiler_provider_make(const char *id,const char *name,const char *executable,const char *target,UmiCompilerFamily family,uint32_t languages,bool cross,bool compilation_database,bool response_files,bool stable_c_abi);
#endif
