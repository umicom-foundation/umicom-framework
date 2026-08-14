/* Umicom Framework | Compiler response files | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_RESPONSE_FILE_H
#define UMICOM_COMPILER_RESPONSE_FILE_H
#include "umicom/compiler/plan.h"
UmiStatus umi_compiler_response_file_encode(const UmiCompilerPlan *plan,char *out_text,size_t capacity);
UmiStatus umi_compiler_response_file_argument(const char *path,char *out_argument,size_t capacity);
#endif
