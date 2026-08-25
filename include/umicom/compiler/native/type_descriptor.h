/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/type_descriptor.h
 *
 * PURPOSE:
 *   Describe portable native compiler types, sizes, alignments and derived pointer/array relationships.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiNativeTypeDescriptor { uint32_t id; UmiNativeTypeKind kind; size_t size; size_t alignment; uint32_t element_type_id; size_t element_count; bool is_const; bool is_volatile; } UmiNativeTypeDescriptor;
UmiStatus umi_nc_type_primitive(UmiNativeTypeDescriptor *type,uint32_t id,UmiNativeTypeKind kind,size_t size,size_t alignment);
UmiStatus umi_nc_type_pointer(UmiNativeTypeDescriptor *type,uint32_t id,uint32_t pointee_type_id,size_t pointer_size);
UmiStatus umi_nc_type_array(UmiNativeTypeDescriptor *type,uint32_t id,uint32_t element_type_id,size_t element_size,size_t element_alignment,size_t count);
bool umi_nc_type_compatible(const UmiNativeTypeDescriptor *a,const UmiNativeTypeDescriptor *b);
#ifdef __cplusplus
}
#endif
#endif
