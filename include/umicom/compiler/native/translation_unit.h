/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/translation_unit.h
 *
 * PURPOSE:
 *   Track top-level declarations and source identity for a single C translation unit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TRANSLATION_UNIT_H
#define UMICOM_COMPILER_NATIVE_TRANSLATION_UNIT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_TOP_LEVEL_DECLARATIONS 128U
typedef struct UmiNativeTranslationUnit { char path[UMI_NC_PATH_CAPACITY]; uint32_t root_node_id; uint32_t declaration_ids[UMI_NC_MAX_TOP_LEVEL_DECLARATIONS]; size_t declaration_count; uint64_t source_fingerprint; } UmiNativeTranslationUnit;
UmiStatus umi_nc_translation_unit_init(UmiNativeTranslationUnit *unit,const char *path,uint32_t root_node_id,uint64_t source_fingerprint);
UmiStatus umi_nc_translation_unit_add_declaration(UmiNativeTranslationUnit *unit,uint32_t node_id);
uint64_t umi_nc_translation_unit_fingerprint(const UmiNativeTranslationUnit *unit);
#ifdef __cplusplus
}
#endif
#endif
