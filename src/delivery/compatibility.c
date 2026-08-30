/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/compatibility.c
 *
 * PURPOSE:
 *   Evaluate Framework ABI and application compatibility before an update is activated.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Compatibility checks stop a newer package from being installed when it requires an ABI the current host cannot provide.
 */

#include "umicom/delivery/compatibility.h"
int umi_compatibility_passed(const UmiCompatibility *compatibility)
{
    return compatibility != NULL &&
           compatibility->available_framework_abi >= compatibility->required_framework_abi &&
           compatibility->available_schema >= compatibility->minimum_schema;
}
