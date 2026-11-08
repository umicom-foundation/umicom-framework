/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/output_file_internal.h
 *
 * PURPOSE:
 *   Share an exclusive output handle with a native child without exposing it publicly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OUTPUT_FILE_INTERNAL_H
#define UMICOM_OUTPUT_FILE_INTERNAL_H
#include "umicom/platform/output_file.h"
/* Private platform composition only. The owner keeps its original non-inherited
 * handle. The caller closes the duplicate after launch (or launch failure).
 * Child writes are raw bytes and do not update UmiOutputFileSnapshot.bytes_written;
 * do not mix parent writes with child writes or infer their count from that field.
 * Close the owning file only after the child has stopped using its duplicate. */
#ifdef _WIN32
UmiStatus UmiOutputFileDuplicateForChild(UmiOutputFile *file, void **outHandle);
#else
/* The duplicate is CLOEXEC and above stderr. dup2 installs it on stderr in the
 * child; the remaining private descriptors are closed before exec. */
UmiStatus UmiOutputFileDuplicateForChild(UmiOutputFile *file, int *outDescriptor);
#endif
#endif
