/* Umicom Framework | Native Assembler provider | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/assembly_provider.h"
#include "provider_defaults.h"
UmiCompilerProvider umi_compiler_assembly_provider(const char *executable,const char *target)
{
    return umi_compiler_provider_make("assembler","Native Assembler",executable,target,UMI_COMPILER_FAMILY_ASSEMBLER,UMI_COMPILER_LANGUAGE_BIT(UMI_COMPILER_LANGUAGE_ASSEMBLY),true,false,true,true);
}
