/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/socket_semantics.h
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SOCKET_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_SOCKET_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtSocketSemantics { bool ipv4; bool ipv6; bool local_domain; bool nonblocking; bool async_completion; bool readiness_polling; } UmiCtSocketSemantics;
UmiCtSupportLevel umi_ct_socket_semantics_support(const UmiCtSocketSemantics *semantics,bool require_ipv6,bool require_local,bool require_async);

#ifdef __cplusplus
}
#endif

#endif
