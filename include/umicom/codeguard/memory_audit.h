/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/memory_audit.h
 *
 * PURPOSE:
 *   Publish the public memory audit contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_MEMORY_AUDIT_H
#define UMICOM_CODEGUARD_MEMORY_AUDIT_H
#include "umicom/codeguard/evidence.h"
/**
 * Provide the codeguard memory audit line operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_memory_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);
#endif
