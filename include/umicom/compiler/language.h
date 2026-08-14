/* Umicom Framework | Compiler language identities | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_LANGUAGE_H
#define UMICOM_COMPILER_LANGUAGE_H
#include "umicom/compiler/common.h"
const char *umi_compiler_language_name(UmiCompilerLanguage language);
const char *umi_compiler_language_default_standard(UmiCompilerLanguage language);
const char *umi_compiler_language_extension(UmiCompilerLanguage language);
UmiCompilerLanguage umi_compiler_language_from_extension(const char *path);
bool umi_compiler_language_uses_c_abi(UmiCompilerLanguage language);
#endif
