/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/vcs/change_list.h
 *
 * PURPOSE:
 *   Project canonical VCS Change List state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_VCS_CHANGE_LIST_H
#define UMICOM_DEVELOPER_OPERATIONS_VCS_CHANGE_LIST_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateVcsChangeList(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
