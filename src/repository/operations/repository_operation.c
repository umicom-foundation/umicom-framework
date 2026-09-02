/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation.c
 *
 * PURPOSE:
 *   Implement define repository operation kinds exposed by the shared command backend.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/repository_operation.h"
/* Translate an operation kind without coupling the model to a CLI parser. */
const char *umi_repository_operation_text(UmiRepositoryOperationKind kind){/* Select the behaviour associated with the requested command or state value. */ switch(kind){case UMI_REPOSITORY_OPERATION_STATUS:return "status";case UMI_REPOSITORY_OPERATION_DOCTOR:return "doctor";case UMI_REPOSITORY_OPERATION_AUDIT:return "audit";case UMI_REPOSITORY_OPERATION_GRAPH:return "graph";case UMI_REPOSITORY_OPERATION_PLAN:return "plan";default:return "unknown";}}
