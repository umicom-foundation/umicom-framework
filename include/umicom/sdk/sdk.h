/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/sdk.h
 *
 * PURPOSE:
 *   Expose the installed-SDK validation and conformance API through one header.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This aggregate header is convenient for tools that need the complete SDK validation surface.
 */
#ifndef UMICOM_SDK_SDK_H
#define UMICOM_SDK_SDK_H
#include "umicom/sdk/component.h"
#include "umicom/sdk/manifest.h"
#include "umicom/sdk/package.h"
#include "umicom/sdk/locator.h"
#include "umicom/sdk/probe.h"
#include "umicom/sdk/compatibility.h"
#include "umicom/sdk/consumer.h"
#include "umicom/sdk/conformance.h"
#include "umicom/sdk/installation.h"
#include "umicom/sdk/report.h"
#include "umicom/sdk/profile.h"
#include "umicom/sdk/target.h"
#include "umicom/sdk/toolchain.h"
#include "umicom/sdk/dependency.h"
#include "umicom/sdk/evidence.h"
#include "umicom/sdk/validator.h"
#endif
