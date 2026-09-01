/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/abi.h
 *
 * PURPOSE:
 *   Expose the complete public ABI hardening API through one convenience header.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Applications may include this aggregate header when they need the complete ABI validation surface.
 */
#ifndef UMICOM_ABI_ABI_H
#define UMICOM_ABI_ABI_H
#include "umicom/abi/version.h"
#include "umicom/abi/hash.h"
#include "umicom/abi/feature.h"
#include "umicom/abi/platform.h"
#include "umicom/abi/symbol.h"
#include "umicom/abi/struct_layout.h"
#include "umicom/abi/descriptor.h"
#include "umicom/abi/contract.h"
#include "umicom/abi/compatibility.h"
#include "umicom/abi/catalogue.h"
#include "umicom/abi/baseline.h"
#include "umicom/abi/deprecation.h"
#include "umicom/abi/binary.h"
#include "umicom/abi/report.h"
#include "umicom/abi/check.h"
#endif
