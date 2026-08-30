/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control_operation.h
 *
 * PURPOSE:
 *   Define source-control operation metadata used by command palettes, menus,
 *   audit surfaces and future AI tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_SOURCE_CONTROL_OPERATION_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_SOURCE_CONTROL_OPERATION_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDeveloperSourceControlOperationKind {
    UMI_DEVELOPER_SOURCE_CONTROL_REFRESH = 1,
    UMI_DEVELOPER_SOURCE_CONTROL_STAGE = 2,
    UMI_DEVELOPER_SOURCE_CONTROL_UNSTAGE = 3,
    UMI_DEVELOPER_SOURCE_CONTROL_STAGE_ALL = 4,
    UMI_DEVELOPER_SOURCE_CONTROL_DISCARD = 5,
    UMI_DEVELOPER_SOURCE_CONTROL_COMMIT = 6,
    UMI_DEVELOPER_SOURCE_CONTROL_FETCH = 7,
    UMI_DEVELOPER_SOURCE_CONTROL_PULL = 8,
    UMI_DEVELOPER_SOURCE_CONTROL_PUSH = 9,
    UMI_DEVELOPER_SOURCE_CONTROL_BRANCH_CREATE = 10,
    UMI_DEVELOPER_SOURCE_CONTROL_BRANCH_CHECKOUT = 11,
    UMI_DEVELOPER_SOURCE_CONTROL_BRANCH_DELETE = 12,
    UMI_DEVELOPER_SOURCE_CONTROL_DIFF = 13
} UmiDeveloperSourceControlOperationKind;

typedef struct UmiDeveloperSourceControlOperationDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *operation_id;
    const char *title;
    const char *description;
    UmiDeveloperSourceControlOperationKind kind;
    int mutates_repository;
    int requires_trust;
    int requires_argument;
} UmiDeveloperSourceControlOperationDefinition;

UmiStatus umi_developer_source_control_operation_validate(
    const UmiDeveloperSourceControlOperationDefinition *definition);

#ifdef __cplusplus
}
#endif

#endif
