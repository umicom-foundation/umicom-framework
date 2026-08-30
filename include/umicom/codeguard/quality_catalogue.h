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
size_t umi_codeguard_quality_catalogue_count(void);
const char *umi_codeguard_quality_catalogue_at(size_t index);
#endif
