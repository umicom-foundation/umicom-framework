/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation_request.c
 *
 * PURPOSE:
 *   Implement represent a repository operation request independently of CLI argument parsing.
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

#include "umicom/repository/repository_operation_request.h"
#include <string.h>
/* Initialise one portable operation request with no implicit mutations. */
void umi_repository_operation_request_init(UmiRepositoryOperationRequest *request,UmiRepositoryOperationKind kind,const char *repository_root){if(!request)return;(void)memset(request,0,sizeof(*request));request->kind=kind;request->repository_root=(repository_root&&repository_root[0])?repository_root:".";}
