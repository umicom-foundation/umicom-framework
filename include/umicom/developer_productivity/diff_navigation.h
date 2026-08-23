/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diff_navigation.h
 *
 * PURPOSE:
 *   Navigate next/previous changed rows and hunks in a compare document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_NAVIGATION_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIFF_NAVIGATION_H

#include "umicom/developer_productivity/diff_hunk.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_developer_diff_next_change(
    const UmiDeveloperDiffDocument *document,
    size_t after_row,
    size_t *out_row);

UmiStatus umi_developer_diff_previous_change(
    const UmiDeveloperDiffDocument *document,
    size_t before_row,
    size_t *out_row);

#ifdef __cplusplus
}
#endif

#endif
