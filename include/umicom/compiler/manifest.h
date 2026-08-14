/* Umicom Framework | Polyglot project manifest | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_MANIFEST_H
#define UMICOM_COMPILER_MANIFEST_H
#include "umicom/compiler/request.h"
typedef struct UmiCompilerUnitManifest { char unit_id[UMI_COMPILER_ID_CAPACITY]; char source[UMI_COMPILER_PATH_CAPACITY]; char output[UMI_COMPILER_PATH_CAPACITY]; char provider_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerLanguage language; char exports_abi[UMI_COMPILER_ID_CAPACITY]; bool entry_point; } UmiCompilerUnitManifest;
typedef struct UmiCompilerProjectManifest { char project_id[UMI_COMPILER_ID_CAPACITY]; char name[UMI_COMPILER_NAME_CAPACITY]; UmiCompilerUnitManifest units[UMI_COMPILER_MAX_UNITS]; size_t unit_count; char target[UMI_COMPILER_NAME_CAPACITY]; uint64_t revision; } UmiCompilerProjectManifest;
UmiStatus umi_compiler_manifest_init(UmiCompilerProjectManifest *manifest,const char *project_id,const char *name,const char *target);
UmiStatus umi_compiler_manifest_add_unit(UmiCompilerProjectManifest *manifest,const UmiCompilerUnitManifest *unit);
const UmiCompilerUnitManifest *umi_compiler_manifest_find_unit(const UmiCompilerProjectManifest *manifest,const char *unit_id);
UmiStatus umi_compiler_manifest_validate(const UmiCompilerProjectManifest *manifest,char *out_reason,size_t capacity);
#endif
