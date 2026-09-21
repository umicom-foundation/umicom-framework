/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/cpack_provider.h
 * PURPOSE: Produce packages from the selected project's existing CMake install rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_CPACK_PROVIDER_H
#define UMICOM_BUILD_CPACK_PROVIDER_H
#include "umicom/build/provider.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Return the ZIP packaging provider. Its command uses CPackConfig.cmake in the
 * selected build directory, emits packages below that directory's packages/
 * child and asks CPack for SHA-256 sidecars. No process starts in this function.
 * The project must include(CPack) and supply install rules. A provider command
 * alone does not build or test: use umi_build_project_session_submit(PACKAGE)
 * for Configure/Build-all/Test/Package with trust and cancellation checks.
 * See examples/development_workflow/main.c for a complete project exercise. */
UmiBuildProvider UmiBuildCPackProvider(void);
#ifdef __cplusplus
}
#endif
#endif
