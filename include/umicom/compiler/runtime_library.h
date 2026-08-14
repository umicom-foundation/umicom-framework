/* Umicom Framework | Runtime library contracts | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_RUNTIME_LIBRARY_H
#define UMICOM_COMPILER_RUNTIME_LIBRARY_H
#include "umicom/compiler/abi.h"
#define UMI_COMPILER_MAX_RUNTIME_LIBRARIES 32U
typedef struct UmiCompilerRuntimeLibrary { char runtime_id[UMI_COMPILER_ID_CAPACITY]; char path[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerLanguage language; UmiCompilerAbiProfile abi; bool static_link; bool available; } UmiCompilerRuntimeLibrary;
typedef struct UmiCompilerRuntimeCatalogue { UmiCompilerRuntimeLibrary items[UMI_COMPILER_MAX_RUNTIME_LIBRARIES]; size_t count; uint64_t revision; } UmiCompilerRuntimeCatalogue;
UmiStatus umi_compiler_runtime_register(UmiCompilerRuntimeCatalogue *catalogue,const UmiCompilerRuntimeLibrary *runtime);
const UmiCompilerRuntimeLibrary *umi_compiler_runtime_resolve(const UmiCompilerRuntimeCatalogue *catalogue,UmiCompilerLanguage language,const UmiCompilerAbiProfile *abi);
#endif
