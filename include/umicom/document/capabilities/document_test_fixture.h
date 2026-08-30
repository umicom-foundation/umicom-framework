/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/capabilities/document_test_fixture.h
 *
 * PURPOSE:
 *   Declare the Framework-owned Document Test Fixture document capability descriptor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TEST_FIXTURE_H
#define UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TEST_FIXTURE_H

#include "umicom/document/capability.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_CAPABILITY_DOCUMENT_TEST_FIXTURE "umicom.document.integration.document_test_fixture"

const UmiDocumentCapabilityDescriptor *
umi_document_capability_document_test_fixture(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_CAPABILITIES_DOCUMENT_TEST_FIXTURE_H */
