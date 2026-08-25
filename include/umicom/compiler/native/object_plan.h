/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/object_plan.h
 *
 * PURPOSE:
 *   Aggregate object sections, relocations and object-format intent before backend serialization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_OBJECT_PLAN_H
#define UMICOM_COMPILER_NATIVE_OBJECT_PLAN_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/object_section.h"
#include "umicom/compiler/native/relocation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_OBJECT_SECTIONS 16U
#define UMI_NC_MAX_RELOCATIONS 128U
typedef struct UmiNativeObjectPlan { UmiNativeObjectFormat format; UmiNativeObjectSection sections[UMI_NC_MAX_OBJECT_SECTIONS]; size_t section_count; UmiNativeRelocation relocations[UMI_NC_MAX_RELOCATIONS]; size_t relocation_count; char entry_symbol[UMI_NC_NAME_CAPACITY]; } UmiNativeObjectPlan;
void umi_nc_object_plan_init(UmiNativeObjectPlan *plan,UmiNativeObjectFormat format);
UmiStatus umi_nc_object_plan_add_section(UmiNativeObjectPlan *plan,const UmiNativeObjectSection *section);
UmiStatus umi_nc_object_plan_add_relocation(UmiNativeObjectPlan *plan,const UmiNativeRelocation *relocation);
size_t umi_nc_object_plan_total_size(const UmiNativeObjectPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
