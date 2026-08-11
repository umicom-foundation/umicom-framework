/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/browser.c
 *
 * PURPOSE:
 *   Define the stable embedded-browser function-table contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/browser.h"
UmiStatus umi_browser_validate(const UmiBrowserV1 *browser){if(browser==NULL||browser->instance==NULL||browser->navigate==NULL||browser->current_url==NULL||browser->destroy==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
