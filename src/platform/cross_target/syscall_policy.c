/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/syscall_policy.c
 *
 * PURPOSE:
 *   Apply allow/deny policy to syscall descriptors using explicit flag and number ranges.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/syscall_policy.h"

/*
 * Provide the ct syscall policy allows operation used by this module and its client
 * applications.
 */
bool umi_ct_syscall_policy_allows(const UmiCtSyscallPolicy*p,const UmiCtSyscallDescriptor*d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||d==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->number<p->minimum_number||d->number>p->maximum_number)return !p->default_allow?false:false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((d->flags&p->denied_flags)!=0U)return false;return p->default_allow;}
