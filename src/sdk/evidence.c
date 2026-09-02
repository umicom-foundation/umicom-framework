/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/evidence.c
 *
 * PURPOSE:
 *   Collect named pass/fail evidence produced by SDK validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/evidence.h"
#include <stddef.h>
/* Check that sdk evidence satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_evidence_validate(const UmiSdkEvidence *e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||e->check_id==NULL||e->check_id[0]=='\0'||e->detail==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
