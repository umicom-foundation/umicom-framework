/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/translation_unit.h
 *
 * PURPOSE:
 *   Track top-level declarations and source identity for a single C translation unit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the native translation unit data shared with callers of this public contract.
 */
typedef struct UmiNativeTranslationUnit { char path[UMI_NC_PATH_CAPACITY]; uint32_t root_node_id; uint32_t declaration_ids[UMI_NC_MAX_TOP_LEVEL_DECLARATIONS]; size_t declaration_count; uint64_t source_fingerprint; } UmiNativeTranslationUnit;
/**
 * Initialise nc translation unit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_translation_unit_init(UmiNativeTranslationUnit *unit,const char *path,uint32_t root_node_id,uint64_t source_fingerprint);
/**
 * Provide the nc translation unit add declaration operation used by this module and its
 * client applications.
 */
UmiStatus umi_nc_translation_unit_add_declaration(UmiNativeTranslationUnit *unit,uint32_t node_id);
/**
 * Provide the nc translation unit fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_translation_unit_fingerprint(const UmiNativeTranslationUnit *unit);
#ifdef __cplusplus
}
#endif
#endif
