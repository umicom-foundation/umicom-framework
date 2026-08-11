/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/install_plan.h
 *
 * PURPOSE:
 *   Build a bounded sequence of installation operations before modifying an installation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * An install plan is inspectable and testable, which makes package installation safer than scattered file-copy commands.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_INSTALL_PLAN_H
#define INCLUDE_UMICOM_DELIVERY_INSTALL_PLAN_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiInstallOperationKind {
    UMI_INSTALL_CREATE_DIRECTORY = 1,
    UMI_INSTALL_COPY_FILE = 2,
    UMI_INSTALL_REMOVE_PATH = 3,
    UMI_INSTALL_SWITCH_GENERATION = 4
} UmiInstallOperationKind;

typedef struct UmiInstallOperation {
    UmiInstallOperationKind kind;
    char source[UMI_DELIVERY_PATH_CAPACITY];
    char destination[UMI_DELIVERY_PATH_CAPACITY];
} UmiInstallOperation;

typedef struct UmiInstallPlan {
    UmiInstallOperation operations[UMI_DELIVERY_MAX_OPERATIONS];
    size_t count;
} UmiInstallPlan;

void umi_install_plan_init(UmiInstallPlan *plan);
UmiStatus umi_install_plan_add(UmiInstallPlan *plan,
                               UmiInstallOperationKind kind,
                               const char *source,
                               const char *destination);

#ifdef __cplusplus
}
#endif

#endif
