/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/terminal_split.h
 *
 * PURPOSE:
 *   Model terminal split groups independently from GTK widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_SPLIT_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_TERMINAL_SPLIT_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TERMINAL_SPLIT_CAPACITY 32U

typedef enum UmiDeveloperTerminalSplitOrientation {
    UMI_DEVELOPER_TERMINAL_SPLIT_HORIZONTAL = 1,
    UMI_DEVELOPER_TERMINAL_SPLIT_VERTICAL = 2
} UmiDeveloperTerminalSplitOrientation;

typedef struct UmiDeveloperTerminalSplit {
    char split_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char first_session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char second_session_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    UmiDeveloperTerminalSplitOrientation orientation;
    double ratio;
} UmiDeveloperTerminalSplit;

typedef struct UmiDeveloperTerminalSplitModel {
    UmiDeveloperTerminalSplit splits[UMI_DEVELOPER_TERMINAL_SPLIT_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiDeveloperTerminalSplitModel;

void umi_developer_terminal_split_model_init(
    UmiDeveloperTerminalSplitModel *model);

UmiStatus umi_developer_terminal_split_model_add(
    UmiDeveloperTerminalSplitModel *model,
    const UmiDeveloperTerminalSplit *split);

#ifdef __cplusplus
}
#endif

#endif
