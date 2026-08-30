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

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/evidence.h"
#include <stddef.h>
UmiStatus umi_sdk_evidence_validate(const UmiSdkEvidence *e){if(e==NULL||e->check_id==NULL||e->check_id[0]=='\0'||e->detail==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
