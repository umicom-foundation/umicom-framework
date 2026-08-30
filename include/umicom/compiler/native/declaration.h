/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/declaration.h
 *
 * PURPOSE:
 *   Describe top-level and local C declarations independently from parser implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_DECLARATION_H
#define UMICOM_COMPILER_NATIVE_DECLARATION_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeDeclarationKind { UMI_NC_DECL_VARIABLE=1, UMI_NC_DECL_FUNCTION=2, UMI_NC_DECL_TYPEDEF=3, UMI_NC_DECL_STRUCT=4, UMI_NC_DECL_UNION=5, UMI_NC_DECL_ENUM=6 } UmiNativeDeclarationKind;
typedef struct UmiNativeDeclaration { uint32_t node_id; UmiNativeDeclarationKind kind; char name[UMI_NC_NAME_CAPACITY]; uint32_t type_id; bool external_linkage; bool internal_linkage; bool definition; } UmiNativeDeclaration;
UmiStatus umi_nc_declaration_init(UmiNativeDeclaration *declaration,uint32_t node_id,UmiNativeDeclarationKind kind,const char *name,uint32_t type_id);
UmiStatus umi_nc_declaration_validate(const UmiNativeDeclaration *declaration);
#ifdef __cplusplus
}
#endif
#endif
