/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral primitives for the reusable developer
 *   workbench consumed by Umicom Studio and future thin development products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_TYPES_H
#define UMICOM_DEVELOPER_WORKBENCH_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_WORKBENCH_API_VERSION 1U
#define UMI_DEVELOPER_WORKBENCH_ID_CAPACITY 192U
#define UMI_DEVELOPER_WORKBENCH_TITLE_CAPACITY 256U
#define UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY 512U
#define UMI_DEVELOPER_WORKBENCH_PATH_CAPACITY 2048U
#define UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY 256U
#define UMI_DEVELOPER_WORKBENCH_MAX_COMMANDS 256U
#define UMI_DEVELOPER_WORKBENCH_MAX_CONFIGURATIONS 32U
#define UMI_DEVELOPER_WORKBENCH_MAX_RESULTS 256U
#define UMI_DEVELOPER_WORKBENCH_MAX_PERSPECTIVES 32U
#define UMI_DEVELOPER_WORKBENCH_MAX_SURFACES 32U
#define UMI_DEVELOPER_WORKBENCH_HISTORY_CAPACITY 256U

typedef enum UmiDeveloperWorkbenchCommandAction {
    UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL = 0,
    UMI_DEVELOPER_WORKBENCH_ACTION_CONFIGURE = 1,
    UMI_DEVELOPER_WORKBENCH_ACTION_BUILD = 2,
    UMI_DEVELOPER_WORKBENCH_ACTION_REBUILD = 3,
    UMI_DEVELOPER_WORKBENCH_ACTION_CLEAN = 4,
    UMI_DEVELOPER_WORKBENCH_ACTION_TEST = 5,
    UMI_DEVELOPER_WORKBENCH_ACTION_RUN = 6,
    UMI_DEVELOPER_WORKBENCH_ACTION_INSTALL = 7,
    UMI_DEVELOPER_WORKBENCH_ACTION_PACKAGE = 8,
    UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL = 9
} UmiDeveloperWorkbenchCommandAction;

typedef uint32_t UmiDeveloperWorkbenchCommandFlags;
enum {
    UMI_DEVELOPER_WORKBENCH_COMMAND_NONE = 0U,
    UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE = 1U << 0,
    UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST = 1U << 1,
    UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND = 1U << 2,
    UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE = 1U << 3,
    UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED = 1U << 4
};

typedef struct UmiDeveloperWorkbenchCommandDefinition {
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    UmiDeveloperWorkbenchCommandAction action;
    UmiDeveloperWorkbenchCommandFlags flags;
} UmiDeveloperWorkbenchCommandDefinition;

const char *umi_developer_workbench_command_action_text(
    UmiDeveloperWorkbenchCommandAction action);

UmiStatus umi_developer_workbench_command_definition_validate(
    const UmiDeveloperWorkbenchCommandDefinition *definition);

#ifdef __cplusplus
}
#endif

#endif
