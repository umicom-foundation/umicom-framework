/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/consumer.c
 *
 * PURPOSE:
 *   Describe one external application that consumes the installed Framework SDK.
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
#include "umicom/sdk/consumer.h"
#include <stddef.h>
/* Check that sdk consumer satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_consumer_validate(const UmiSdkConsumer *c){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->application_id==NULL||c->application_id[0]=='\0'||c->requirement.required_abi==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->target_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->required_targets[i]==NULL||c->required_targets[i][0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
