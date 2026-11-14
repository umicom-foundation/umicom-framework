/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/symbol_names.h
 * PURPOSE:
 *   Qualify private Setup Centre helper symbols without changing public APIs.
 * EDUCATIONAL OVERVIEW:
 *   Static libraries share one final linker namespace. Short cross-file helper
 *   names can collide even when their C source files are in different folders.
 *   Only private emitted names change; implementation and call-site spellings stay.
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SETUP_CENTRE_SYMBOL_NAMES_H
#define UMICOM_SETUP_CENTRE_SYMBOL_NAMES_H
#define ScReport UmiSetupCentreInternalReport
#define ScTextInit UmiSetupCentreInternalTextInit
#define ScTextFree UmiSetupCentreInternalTextFree
#define ScPrint UmiSetupCentreInternalPrint
#define ScToken UmiSetupCentreInternalToken
#define ScUtf8 UmiSetupCentreInternalUtf8
#define ScHashValid UmiSetupCentreInternalHashValid
#define ScNumber UmiSetupCentreInternalNumber
#define ScJoin UmiSetupCentreInternalJoin
#define ScEqualFold UmiSetupCentreInternalEqualFold
#define ScRead UmiSetupCentreInternalRead
#define ScWriteNew UmiSetupCentreInternalWriteNew
#define ScParentCheck UmiSetupCentreInternalParentCheck
#define ScMakeDirectory UmiSetupCentreInternalMakeDirectory
#define ScParents UmiSetupCentreInternalParents
#define ScCopy UmiSetupCentreInternalCopy
#define ScDigest UmiSetupCentreInternalDigest
#define ScReadAt UmiSetupCentreInternalReadAt
#define ScHeaderRead UmiSetupCentreInternalHeaderRead
#define ScFileRegular UmiSetupCentreInternalFileRegular
#define ScFreeSpace UmiSetupCentreInternalFreeSpace
#define ScAddFile UmiSetupCentreInternalAddFile
#define ScEncode UmiSetupCentreInternalEncode
#define ScDecode UmiSetupCentreInternalDecode
#define ScImportReports UmiSetupCentreInternalImportReports
#define ScSelected UmiSetupCentreInternalSelected
#define ScHostProgram UmiSetupCentreInternalHostProgram
#define ScKernel UmiSetupCentreInternalKernel
#endif
