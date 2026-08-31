/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/quality_catalogue.h
 *
 * PURPOSE:
 *   Publish the public quality catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_QUALITY_CATALOGUE_H
#define UMICOM_CODEGUARD_QUALITY_CATALOGUE_H

#include <stddef.h>

/* Return the number of reusable quality capabilities currently published. */
size_t umi_codeguard_quality_catalogue_count(void);

/* Borrow one stable capability ID, or return NULL outside the catalogue. */
const char *umi_codeguard_quality_catalogue_at(size_t index);

#endif
