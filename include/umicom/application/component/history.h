/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/history.h
 *
 * PURPOSE:
 *   Record reversible workspace changes so a layout editor can offer familiar
 *   undo and redo commands without storing large copies of the whole layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_HISTORY_H
#define UMICOM_APPLICATION_COMPONENT_HISTORY_H

#include "umicom/application/component/draft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY 64U
#define UMI_APPLICATION_COMPONENT_CHANGE_SUMMARY_CAPACITY 160U

/**
 * List the named application component change kind values accepted by this public
 * contract.
 */
typedef enum UmiApplicationComponentChangeKind {
  UMI_APPLICATION_COMPONENT_CHANGE_ADD = 1,
  UMI_APPLICATION_COMPONENT_CHANGE_REMOVE = 2,
  UMI_APPLICATION_COMPONENT_CHANGE_MOVE = 3,
  UMI_APPLICATION_COMPONENT_CHANGE_VISIBILITY = 4,
  UMI_APPLICATION_COMPONENT_CHANGE_REGION = 5,
  UMI_APPLICATION_COMPONENT_CHANGE_WEIGHT = 6,
  UMI_APPLICATION_COMPONENT_CHANGE_TITLE = 7
} UmiApplicationComponentChangeKind;

/**
 * Represent the application component change data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentChange {
  UmiApplicationComponentChangeKind kind;
  char summary[UMI_APPLICATION_COMPONENT_CHANGE_SUMMARY_CAPACITY];
  size_t before_index;
  size_t after_index;
  UmiApplicationComponentDraftSlot before_slot;
  UmiApplicationComponentDraftSlot after_slot;
  char before_title[UMI_APPLICATION_COMPONENT_TITLE_CAPACITY];
  char after_title[UMI_APPLICATION_COMPONENT_TITLE_CAPACITY];
} UmiApplicationComponentChange;

/**
 * Represent the application component history data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentHistory {
  UmiApplicationComponentChange changes[UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY];
  size_t count;
  size_t cursor;
  uint64_t revision;
} UmiApplicationComponentHistory;

/**
 * Initialise application component history from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_component_history_init(UmiApplicationComponentHistory *history);
/**
 * Provide the application component history record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_history_record(UmiApplicationComponentHistory *history,
                                                   const UmiApplicationComponentChange *change);
/**
 * Provide the application component history can undo operation used by this module and its
 * client applications.
 */
int umi_application_component_history_can_undo(const UmiApplicationComponentHistory *history);
/**
 * Provide the application component history can redo operation used by this module and its
 * client applications.
 */
int umi_application_component_history_can_redo(const UmiApplicationComponentHistory *history);
/**
 * Provide the application component history undo operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_history_undo(UmiApplicationComponentHistory *history,
                                                 UmiApplicationComponentWorkspaceDraft *draft);
/**
 * Provide the application component history redo operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_history_redo(UmiApplicationComponentHistory *history,
                                                 UmiApplicationComponentWorkspaceDraft *draft);

#ifdef __cplusplus
}
#endif

#endif
