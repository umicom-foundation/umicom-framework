/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/socket_semantics.c
 *
 * PURPOSE:
 *   Describe IP/local socket and asynchronous I/O capabilities for Framework networking portability.
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

#include "umicom/platform/cross_target/socket_semantics.h"

UmiCtSupportLevel umi_ct_socket_semantics_support(const UmiCtSocketSemantics*s,bool v6,bool local,bool async){if(s==NULL||!s->ipv4||!s->nonblocking)return UMI_CT_SUPPORT_NONE;if((v6&&!s->ipv6)||(local&&!s->local_domain)||(async&&!(s->async_completion||s->readiness_polling)))return UMI_CT_SUPPORT_DEGRADED;return UMI_CT_SUPPORT_NATIVE;}
