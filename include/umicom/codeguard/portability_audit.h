/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/portability_audit.h
 *
 * PURPOSE:
 *   Publish the public portability audit contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_PORTABILITY_AUDIT_H
#define UMICOM_CODEGUARD_PORTABILITY_AUDIT_H
#include "umicom/codeguard/evidence.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the codeguard portability is adapter path operation used by this module and its
 * client applications.
 */
bool umi_codeguard_portability_is_adapter_path(const char *path);
/**
 * Provide the codeguard portability audit line operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_portability_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);

#ifdef __cplusplus
}
#endif
#endif
