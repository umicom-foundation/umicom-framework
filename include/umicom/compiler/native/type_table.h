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
/**
 * Represent the native type table data shared with callers of this public contract.
 */
typedef struct UmiNativeTypeTable { UmiNativeTypeDescriptor items[UMI_NC_MAX_TYPES]; size_t count; uint64_t revision; } UmiNativeTypeTable;
/**
 * Initialise nc type table from caller-provided values so later operations receive a known
 * state.
 */
void umi_nc_type_table_init(UmiNativeTypeTable *table);
/**
 * Add nc type table only after its inputs and available capacity have been checked.
 */
UmiStatus umi_nc_type_table_add(UmiNativeTypeTable *table,const UmiNativeTypeDescriptor *type);
/**
 * Find nc type table while leaving the underlying catalogue or model owned by this module.
 */
const UmiNativeTypeDescriptor *umi_nc_type_table_find(const UmiNativeTypeTable *table,uint32_t id);
/**
 * Provide the nc type table find compatible operation used by this module and its client
 * applications.
 */
const UmiNativeTypeDescriptor *umi_nc_type_table_find_compatible(const UmiNativeTypeTable *table,const UmiNativeTypeDescriptor *type);
#ifdef __cplusplus
}
#endif
#endif
