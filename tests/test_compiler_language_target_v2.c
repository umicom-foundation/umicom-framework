/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <string.h>
int main(void) { UmiCompilerTarget target; assert(umi_compiler_language_from_extension("engine.cpp") == UMI_COMPILER_LANGUAGE_CPP); assert(strcmp(umi_compiler_language_default_standard(UMI_COMPILER_LANGUAGE_C),"c23") == 0); assert(umi_compiler_target_parse("x86_64-pc-windows-gnu",&target) == UMI_STATUS_OK); assert(target.object_format == UMI_COMPILER_OBJECT_COFF); assert(target.pointer_width == 64U); return 0; }
