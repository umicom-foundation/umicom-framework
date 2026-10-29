/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_text.h
 *
 * PURPOSE:
 *   Search verified offline lesson bodies through shared reading and Editor services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEACHER_LEARNING_TEXT_H
#define UMICOM_TEACHER_LEARNING_TEXT_H
#include "umicom/teacher/learning_pack.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LEARNING_TEXT_DOCUMENTS_MAX (UMI_LEARNING_SHELF_MAX_PACKS * UMI_LEARNING_PACK_MAX_RESOURCES)
#define UMI_LEARNING_TEXT_SOURCE_MAX (1024U * 1024U)
#define UMI_LEARNING_TEXT_DOCUMENT_MAX 65536U
#define UMI_LEARNING_TEXT_TOTAL_MAX (64U * 1024U * 1024U)
#define UMI_LEARNING_TEXT_QUERY_CAPACITY 256U
#define UMI_LEARNING_TEXT_PREVIEW_CAPACITY 256U
#define UMI_LEARNING_TEXT_RESULTS_MAX 256U

typedef int (*UmiLearningTextCancellation)(void *user);
typedef struct UmiLearningTextLimits {
    size_t maximum_documents, maximum_source_bytes;
    size_t maximum_document_bytes, maximum_total_bytes;
} UmiLearningTextLimits;
typedef struct UmiLearningTextProblem {
    UmiStatus status;
    char pack_id[UMI_TEACHER_ID_CAPACITY];
    char resource_id[UMI_TEACHER_ID_CAPACITY];
} UmiLearningTextProblem;
typedef struct UmiLearningTextIndexInfo {
    uint64_t shelf_generation;
    size_t pack_count, document_count, text_bytes, excluded_images;
} UmiLearningTextIndexInfo;
typedef struct UmiLearningTextQuery {
    char phrase[UMI_LEARNING_TEXT_QUERY_CAPACITY];
    char product[UMI_TEACHER_ID_CAPACITY];
    char locale[UMI_LEARNING_PACK_LOCALE_CAPACITY];
    /* Zero accepts every indexed kind. Images are never indexed. */
    unsigned kind;
    int case_sensitive, whole_word;
    size_t maximum_results;
    UmiLearningTextCancellation cancellation;
    void *cancellation_user;
} UmiLearningTextQuery;
typedef struct UmiLearningTextSearchInfo {
    uint64_t shelf_generation;
    size_t examined_documents, matching_documents, result_count;
    int truncated;
} UmiLearningTextSearchInfo;
typedef struct UmiLearningTextHit {
    UmiLearningPackInfo pack;
    UmiLearningPackResource resource;
    /* Offsets refer to the verified source text or the HTML reading projection,
     * never to raw HTML byte positions. End is exclusive. */
    size_t byte_start, byte_end;
    char preview[UMI_LEARNING_TEXT_PREVIEW_CAPACITY];
    int reading_projection, preview_truncated;
} UmiLearningTextHit;
typedef struct UmiLearningTextIndex UmiLearningTextIndex;
typedef struct UmiLearningTextSearch UmiLearningTextSearch;

/* Defaults: 256 documents, 1 MiB per source, 64 KiB per searchable document,
 * 8 MiB total searchable text. Zero is not an unlimited value. */
void UmiLearningTextLimitsInit(UmiLearningTextLimits *limits);
void UmiLearningTextQueryInit(UmiLearningTextQuery *query);
/* Validate bounded fields without reading an index, allocating, changing the
 * query or calling its cancellation callback. Useful before dispatching a job. */
UmiStatus UmiLearningTextQueryValidate(const UmiLearningTextQuery *query);

/* Build one immutable snapshot. Reads only declared, non-image resources and
 * verifies their SHA-256 through PackReadResource. HTML/HTM suffixes, ignoring
 * ASCII case, use the shared inert reading projection; other resources are
 * complete UTF-8 source text. NUL, DEL and controls other than tab/CR/LF refuse.
 * Images are counted but not read. Every other admitted resource must succeed:
 * missing, changed, invalid, oversized or unsupported HTML refuses the entire
 * candidate. No silent partial index, directory scan or network request occurs.
 *
 * Serialize the shelf for this blocking call. A frontend can clone the shelf
 * before transferring it to a worker. Cancellation is checked before each
 * resource and after construction, not inside a blocking filesystem operation.
 * The callback must not mutate/destroy the shelf. The result owns its metadata
 * and indexed text, independently of the shelf, source buffers and files.
 *
 * Failure clears out_index and fills optional problem with the status and
 * known resource identity. Success clears problem. No absolute path is exported.
 * Use separate owning output storage; this does not replace an existing index.
 * It verifies consistency with admitted hashes, not publisher authenticity. */
UmiStatus UmiLearningTextIndexCreate(const UmiLearningShelf *shelf,
    const UmiLearningTextLimits *limits, UmiLearningTextCancellation cancellation,
    void *user, UmiLearningTextIndex **out_index, UmiLearningTextProblem *problem);
void UmiLearningTextIndexDestroy(UmiLearningTextIndex *index);
UmiStatus UmiLearningTextIndexCopyInfo(const UmiLearningTextIndex *index,
    UmiLearningTextIndexInfo *out);

/* Find the first literal phrase occurrence per document using the shared Editor
 * engine. Empty phrases, unterminated arrays and invalid UTF-8 refuse. ASCII
 * case folding/word boundaries are optional; no Unicode normalization, stemming
 * or regex is implied. Product matches exactly; locale folds ASCII with no
 * fallback. Results follow shelf/manifest order, not relevance ranking.
 * maximum_results is 1..256. Count all matching documents, retaining the first
 * bounded results and explicitly reporting truncation. Cancellation or search
 * errors refuse the whole result; there is no partial-success object.
 *
 * No I/O occurs. Concurrent independent searches may read an index kept alive
 * by their owner. Search results own copied metadata/UTF-8 previews and can
 * outlive the index. Preview starts at the match, ends at CR/LF or capacity,
 * and never splits UTF-8. Reopen through LearningReaderCreateFromShelf using
 * the hit's revision and digest before applying its offsets; an old result is
 * historical evidence, not authority to open changed bytes or execute examples.
 * A shelf generation is meaningful only within the originating shelf lineage.
 * Failure clears out_search. CopyInfo/ResultAt preserve output on refusal. */
UmiStatus UmiLearningTextSearchCreate(const UmiLearningTextIndex *index,
    const UmiLearningTextQuery *query, UmiLearningTextSearch **out_search);
void UmiLearningTextSearchDestroy(UmiLearningTextSearch *search);
UmiStatus UmiLearningTextSearchCopyInfo(const UmiLearningTextSearch *search,
    UmiLearningTextSearchInfo *out);
UmiStatus UmiLearningTextSearchResultAt(const UmiLearningTextSearch *search,
    size_t position, UmiLearningTextHit *out);
#ifdef __cplusplus
}
#endif
#endif
