/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/types.h
 *
 * PURPOSE:
 *   Define common ABI-stable document I/O types shared by providers, codecs,
 *   loaders, savers, working copies, coordinators and frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_TYPES_H
#define UMICOM_DOCUMENT_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_URI_CAPACITY 4096U
#define UMI_DOCUMENT_NAME_CAPACITY 256U
#define UMI_DOCUMENT_LANGUAGE_CAPACITY 64U
#define UMI_DOCUMENT_MIME_CAPACITY 96U
#define UMI_DOCUMENT_MAX_WORKING_COPIES 128U
#define UMI_DOCUMENT_DEFAULT_MAXIMUM_BYTES (16U * 1024U * 1024U)

/**
 * List the named document text encoding values accepted by this public contract.
 */
typedef enum UmiDocumentTextEncoding {
    UMI_DOCUMENT_ENCODING_UTF8 = 0,
    UMI_DOCUMENT_ENCODING_UTF8_BOM = 1,
    UMI_DOCUMENT_ENCODING_UTF16_LE = 2,
    UMI_DOCUMENT_ENCODING_UTF16_BE = 3,
    UMI_DOCUMENT_ENCODING_BINARY = 4,
    UMI_DOCUMENT_ENCODING_UNKNOWN = 5
} UmiDocumentTextEncoding;

/**
 * List the named document line ending values accepted by this public contract.
 */
typedef enum UmiDocumentLineEnding {
    UMI_DOCUMENT_LINE_ENDING_NONE = 0,
    UMI_DOCUMENT_LINE_ENDING_LF = 1,
    UMI_DOCUMENT_LINE_ENDING_CRLF = 2,
    UMI_DOCUMENT_LINE_ENDING_CR = 3,
    UMI_DOCUMENT_LINE_ENDING_MIXED = 4
} UmiDocumentLineEnding;

/**
 * List the named document conflict state values accepted by this public contract.
 */
typedef enum UmiDocumentConflictState {
    UMI_DOCUMENT_CONFLICT_NONE = 0,
    UMI_DOCUMENT_CONFLICT_EXTERNAL_CHANGE = 1,
    UMI_DOCUMENT_CONFLICT_DELETED_EXTERNALLY = 2,
    UMI_DOCUMENT_CONFLICT_WRITE_REJECTED = 3
} UmiDocumentConflictState;

/**
 * Represent the document fingerprint data shared with callers of this public contract.
 */
typedef struct UmiDocumentFingerprint {
    uint64_t content_hash;
    uint64_t byte_count;
    int64_t modified_time_seconds;
    int valid;
} UmiDocumentFingerprint;

/**
 * Represent the document file info data shared with callers of this public contract.
 */
typedef struct UmiDocumentFileInfo {
    char path[UMI_PATH_CAPACITY];
    uint64_t byte_count;
    int64_t modified_time_seconds;
    int exists;
    int regular_file;
    int readable;
    int writable;
} UmiDocumentFileInfo;

/**
 * Represent the document load options data shared with callers of this public contract.
 */
typedef struct UmiDocumentLoadOptions {
    size_t maximum_bytes;
    UmiDocumentTextEncoding fallback_encoding;
    UmiDocumentLineEnding normalise_to;
    int reject_binary;
    int validate_utf8;
} UmiDocumentLoadOptions;

/**
 * Represent the document load result data shared with callers of this public contract.
 */
typedef struct UmiDocumentLoadResult {
    char *text;
    size_t text_length;
    UmiDocumentTextEncoding detected_encoding;
    UmiDocumentLineEnding detected_line_ending;
    size_t lf_count;
    size_t crlf_count;
    size_t cr_count;
    UmiDocumentFingerprint fingerprint;
    int had_bom;
    int binary;
} UmiDocumentLoadResult;

/**
 * Represent the document save options data shared with callers of this public contract.
 */
typedef struct UmiDocumentSaveOptions {
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding line_ending;
    int include_bom;
    int ensure_final_newline;
    int atomic_replace;
} UmiDocumentSaveOptions;

/**
 * Represent the document save result data shared with callers of this public contract.
 */
typedef struct UmiDocumentSaveResult {
    size_t bytes_written;
    UmiDocumentFingerprint fingerprint;
    int atomic_replace_used;
} UmiDocumentSaveResult;

/**
 * Provide the document encoding text operation used by this module and its client
 * applications.
 */
const char *umi_document_encoding_text(UmiDocumentTextEncoding encoding);
/**
 * Provide the document line ending text operation used by this module and its client
 * applications.
 */
const char *umi_document_line_ending_text(UmiDocumentLineEnding line_ending);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_TYPES_H */
