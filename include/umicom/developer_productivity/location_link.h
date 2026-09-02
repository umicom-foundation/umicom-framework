/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/location_link.h
 *
 * PURPOSE:
 *   Parse common file:line:column links from terminal/output text so clicking a
 *   build or terminal line can navigate to the editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_LOCATION_LINK_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_LOCATION_LINK_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read developer location link into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_developer_location_link_parse(
    const char *text,
    UmiDeveloperProductivityLocation *out_location,
    int *out_matched);

#ifdef __cplusplus
}
#endif

#endif
