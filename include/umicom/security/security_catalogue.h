/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/security_catalogue.h
 *
 * PURPOSE:
 *   Publish the public security catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_CATALOGUE_H
#define UMICOM_SECURITY_CATALOGUE_H
#include <stddef.h>
/**
 * Return the number of records represented by security catalogue without changing their
 * state.
 */
size_t umi_security_catalogue_count(void);
/**
 * Find security catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const char *umi_security_catalogue_at(size_t index);
#endif
