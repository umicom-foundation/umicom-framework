/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/ctest_commands.h
 *
 * PURPOSE:
 *   Identify actual CTest executable commands before reusing qualification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_CTEST_COMMANDS_H
#define UMICOM_TEST_PLATFORM_CTEST_COMMANDS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCtestCommandCatalogue UmiCtestCommandCatalogue;
/* Parse a complete CTest JSON v1 document without launching anything.
 * Limits: 128 MiB, 65,536 test identities, 4,095-byte names and 32,767-byte
 * executable paths. The existing strict CTest reader validates all JSON.
 * Duplicate identities and malformed command arrays refuse the whole catalogue.
 * Missing/empty commands remain discoverable but have no executable to verify.
 * Owns copied names and programs; input may be released after return. On failure
 * out is NULL. Commands and arguments are evidence, never shell instructions. */
UmiStatus UmiCtestCommandCatalogueParse(const char *json, size_t length,
    UmiCtestCommandCatalogue **out);
void UmiCtestCommandCatalogueDestroy(UmiCtestCommandCatalogue *catalogue);
size_t UmiCtestCommandCatalogueCount(const UmiCtestCommandCatalogue *catalogue);
/* Exact, case-sensitive test identity lookup. Return a borrowed executable
 * spelling valid until Destroy. NOT_FOUND means absent identity or unavailable
 * command. Failure leaves outProgram unchanged. This is not file identity,
 * authorization, dependency closure or proof that the executable can run. */
UmiStatus UmiCtestCommandCatalogueFind(const UmiCtestCommandCatalogue *catalogue,
    const char *name, const char **outProgram);
#ifdef __cplusplus
}
#endif
#endif
