/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/runtime_library.c
 *
 * PURPOSE:
 *   Implement the runtime library behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Compiler runtime catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/runtime_library.h"
#include <string.h>
/* Add compiler runtime only after its inputs and available capacity have been checked. */
UmiStatus umi_compiler_runtime_register(UmiCompilerRuntimeCatalogue *catalogue,const UmiCompilerRuntimeLibrary *runtime)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || runtime == NULL || runtime->runtime_id[0] == '\0' || runtime->path[0] == '\0' || runtime->language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(catalogue->items[index].runtime_id,runtime->runtime_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->count >= UMI_COMPILER_MAX_RUNTIME_LIBRARIES) return UMI_STATUS_CAPACITY_EXCEEDED; catalogue->items[catalogue->count++] = *runtime; catalogue->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the compiler runtime resolve operation used by this module and its client
 * applications.
 */
const UmiCompilerRuntimeLibrary *umi_compiler_runtime_resolve(const UmiCompilerRuntimeCatalogue *catalogue,UmiCompilerLanguage language,const UmiCompilerAbiProfile *abi)
{ size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue == NULL || abi == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < catalogue->count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (catalogue->items[index].available && catalogue->items[index].language == language && umi_compiler_abi_compatible(&catalogue->items[index].abi,abi)) return &catalogue->items[index]; return NULL; }
