/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/browser/browser.c
 *
 * PURPOSE:
 *   Define the stable embedded-browser function-table contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The embedded-browser layer stays provider neutral. A future WebKit, Edge or Qt adapter can implement the same function table.
 */

#include "umicom/browser/browser.h"
/* Check that browser satisfies its contract before another service relies on it. */
UmiStatus umi_browser_validate(const UmiBrowser *browser){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(browser==NULL||browser->instance==NULL||browser->navigate==NULL||browser->current_url==NULL||browser->destroy==NULL)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
