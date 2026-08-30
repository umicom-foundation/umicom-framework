/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/security_catalogue_v2.h
 *
 * PURPOSE:
 *   Publish the public security catalogue v2 contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_CATALOGUE_V2_H
#define UMICOM_SECURITY_CATALOGUE_V2_H
#include <stddef.h>
size_t umi_security_catalogue_v2_count(void);
const char *umi_security_catalogue_v2_at(size_t index);
#endif
