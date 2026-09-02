/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/type_descriptor.h
 *
 * PURPOSE:
 *   Describe portable native compiler types, sizes, alignments and derived pointer/array relationships.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TYPE_DESCRIPTOR_H
#define UMICOM_COMPILER_NATIVE_TYPE_DESCRIPTOR_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native type descriptor data shared with callers of this public contract.
 */
typedef struct UmiNativeTypeDescriptor { uint32_t id; UmiNativeTypeKind kind; size_t size; size_t alignment; uint32_t element_type_id; size_t element_count; bool is_const; bool is_volatile; } UmiNativeTypeDescriptor;
/**
 * Provide the nc type primitive operation used by this module and its client applications.
 */
UmiStatus umi_nc_type_primitive(UmiNativeTypeDescriptor *type,uint32_t id,UmiNativeTypeKind kind,size_t size,size_t alignment);
/**
 * Provide the nc type pointer operation used by this module and its client applications.
 */
UmiStatus umi_nc_type_pointer(UmiNativeTypeDescriptor *type,uint32_t id,uint32_t pointee_type_id,size_t pointer_size);
/**
 * Provide the nc type array operation used by this module and its client applications.
 */
UmiStatus umi_nc_type_array(UmiNativeTypeDescriptor *type,uint32_t id,uint32_t element_type_id,size_t element_size,size_t element_alignment,size_t count);
/**
 * Provide the nc type compatible operation used by this module and its client
 * applications.
 */
bool umi_nc_type_compatible(const UmiNativeTypeDescriptor *a,const UmiNativeTypeDescriptor *b);
#ifdef __cplusplus
}
#endif
#endif
