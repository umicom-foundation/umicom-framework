![Umicom](../../resources/brand/umicom-logo.svg)

# Publish retrieval scores after validation

Retrieval chooses candidate text for a question. A lexical ranker compares words or substrings; a vector ranker compares numeric directions. A reranker is an optional callback that adjusts candidate scores. Keeping these boundaries in Framework lets applications compose them without duplicating validation or sorting.

## 1. Prepare complete input arrays

Use [the chunk and embedding constructors](ai-vector-admission.md) before passing records to retrieval. If another component supplies public structures, call umi_ai_retrieval_chunks_valid for chunks or umi_ai_retrieval_results_valid for scored results.

Both validators admit the entire active array. Chunk strings must terminate inside their fields. Result scores must be finite. Zero count permits a null array, and impossible byte extents are refused before iteration. The caller still has to provide an actual allocation large enough for the advertised count; a C pointer does not encode its allocation size.

~~~c
if (!umi_ai_retrieval_chunks_valid(chunks, chunk_count))
    return UMI_STATUS_INVALID_ARGUMENT;
size_t count = umi_ai_retrieval_rank(
    "target", chunks, chunk_count, results, result_capacity);
~~~

Lexical ranking validates every chunk before writing any result. A malformed final chunk therefore cannot leave valid-looking earlier results. Equal scores retain input order. The standalone lexical scorer still accepts ordinary terminated C strings; it performs case-insensitive substring matching using the current C locale, not Unicode linguistic analysis.

## 2. Define a finite, borrowed-input callback

~~~c
static double emphasize_match(const char *query,
                              const UmiAiChunk *chunk,
                              double base_score,
                              void *user_data)
{
    (void)query;
    (void)user_data;
    /* The record is borrowed. Read it during this call; do not retain its address. */
    return chunk->document_id[0] != '\0' ? base_score + 0.25 : base_score;
}
~~~

A callback receives an already admitted chunk and finite base score. Return a finite score, including a negative score when that is part of your policy. Do not cast away const, retain the temporary chunk pointer, mutate the result owner or reenter a mutation of that owner. External side effects, such as a counter in user_data, cannot be rolled back if a later callback fails.

The service snapshots the callback function and user-data pointer before invocation. Replacing the caller's descriptor during a callback does not change the function used for the remaining entries.

## 3. Publish scores as one operation

~~~c
UmiAiReranker reranker = {emphasize_match, NULL};
UmiStatus status = umi_ai_reranker_apply(
    &reranker, "target", results, count);
if (status != UMI_STATUS_OK)
    return status;
~~~

The reranker first validates every input result, before calling user code. With a callback, it copies the array to owned temporary storage, computes every score, sorts the candidate array and finally publishes it. A non-finite callback result returns INVALID_STATE. Invalid input returns INVALID_ARGUMENT. Allocation failure returns OUT_OF_MEMORY.

These refusal paths preserve the complete result array, provided the callback respects its ownership contract. Callback calls already made remain observable to its owner. A null callback performs the existing stable descending sort without allocating temporary storage.

## 4. Combine lexical and vector candidates

~~~c
UmiAiHybridRetrievalSettings settings =
    umi_ai_hybrid_retrieval_settings_default();
settings.lexical_weight = 0.75;
settings.vector_weight = 0.25;
size_t count = umi_ai_hybrid_retrieval_rank(
    "target", chunks, chunk_count, &store, &query_embedding,
    &settings, &reranker, results, result_capacity);
~~~

The hybrid path admits all lexical chunks and finite weights before composing candidates. When both vector arguments are supplied, it also validates the whole vector store and query embedding. Invalid vector data is refused instead of silently returning a lexical-only blend. Supplying either vector argument as null retains the documented lexical fallback.

The existing signed-weight behavior and candidate-limit clamp remain. Weights are not automatically normalized. Even finite weights can overflow when contributions for one chunk are added; the shared result validator refuses that non-finite merged score before callbacks and output publication.

These APIs return a count, so zero can mean no matches or refusal. Use explicit validators and the reranker's status when your user interface needs more detailed feedback. Output buffers must have their advertised capacities and must not overlap source buffers. Synchronize mutation outside these services.

## 5. Qualify the workflow

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ai-retrieval-publication-test umicom-ai-vector-admission-test umicom-ai-retrieval-tests umicom-ai-hybrid-retrieval-tests
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.](ai[.](retrieval_publication|vector_admission|hybrid_retrieval)|ai_retrieval)$"
~~~

The authored coverage checks malformed late records, impossible extents, stable ties, negative scores, invalid weights, overflowing blends, callback descriptor capture and complete array preservation after a later callback failure.

No compiler, test or application execution has been performed for these changes. Allocation-failure injection, provider integration, concurrent application behavior and retrieval quality remain unverified.
