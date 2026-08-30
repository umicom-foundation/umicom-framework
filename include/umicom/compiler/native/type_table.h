/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/type_table.h
 *
 * PURPOSE:
 *   Maintain canonical type descriptors with stable IDs and structural lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TYPE_TABLE_H
#define UMICOM_COMPILER_NATIVE_TYPE_TABLE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/type_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_TYPES 256U
typedef struct UmiNativeTypeTable { UmiNativeTypeDescriptor items[UMI_NC_MAX_TYPES]; size_t count; uint64_t revision; } UmiNativeTypeTable;
void umi_nc_type_table_init(UmiNativeTypeTable *table);
UmiStatus umi_nc_type_table_add(UmiNativeTypeTable *table,const UmiNativeTypeDescriptor *type);
const UmiNativeTypeDescriptor *umi_nc_type_table_find(const UmiNativeTypeTable *table,uint32_t id);
const UmiNativeTypeDescriptor *umi_nc_type_table_find_compatible(const UmiNativeTypeTable *table,const UmiNativeTypeDescriptor *type);
#ifdef __cplusplus
}
#endif
#endif
