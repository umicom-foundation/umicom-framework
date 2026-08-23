/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/builtin_source_control_operations.h
 *
 * PURPOSE:
 *   Enumerate built-in source-control operation definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_SOURCE_CONTROL_OPERATIONS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_BUILTIN_SOURCE_CONTROL_OPERATIONS_H

#include "umicom/developer_productivity/source_control_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_builtin_source_control_operation_count(void);

const UmiDeveloperSourceControlOperationDefinition *
umi_developer_builtin_source_control_operation_at(size_t index);

#ifdef __cplusplus
}
#endif

#endif
