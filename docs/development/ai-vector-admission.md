![Umicom](../../resources/brand/umicom-logo.svg)

# Admit AI records before retrieval

An embedding is a short vector of numbers. The reference vector store associates each vector with a text chunk and ranks entries by cosine similarity, which compares vector direction. These operations are local mathematical and storage services. They do not train a model, contact a provider or prove that a result is relevant.

## 1. Construct a chunk and embedding

~~~c
#include "umicom/ai/vector_store.h"

UmiAiChunk chunk;
UmiAiEmbedding embedding;
const float direction[] = {1.0F, 0.0F};
UmiStatus status = umi_ai_chunk_set(&chunk, "manual", 0U, 0U, "Open a project");
if (status != UMI_STATUS_OK)
    return status;
status = umi_ai_embedding_set(&embedding, direction, 2U);
if (status != UMI_STATUS_OK)
    return status;
~~~

The chunk constructor checks text capacity, generated identifier capacity and byte-offset addition before publishing the record. A document ID may fit its field while the generated ID, which adds a colon and index, does not. Refusal leaves the complete previous chunk unchanged. Empty strings remain permitted.

Both constructors allow input borrowed from their destination. For example, resetting an embedding from its own first two values captures those values before publishing the replacement. A rejected non-finite component also leaves the previous embedding unchanged.

The caller must provide readable input: the requested number of floats, or text through its terminator or the documented field capacity. Bounded scans cannot establish whether an arbitrary pointer is valid.

## 2. Admit records received from another component

Public C structures can be filled without constructors. Use umi_ai_embedding_valid to check a nonzero dimension within the fixed capacity and finite active components. Inactive array slots are ignored. A vector containing only zeros is valid storage, although its cosine score is neutral.

Use umi_ai_chunk_storage_valid to check termination of its three text fields. This deliberately permits empty metadata and does not certify identity, source provenance or offset meaning. Existing clients that construct a chunk with an ID and text, leaving offsets and document ID zero, remain supported.

A storage validator is a boundary for safe consumption, not a claim that the content is useful or trustworthy.

## 3. Add entries through the store

~~~c
/* Keep this bounded owner off a small thread stack. A real application can
 * allocate one owned instance for each retrieval session. */
static UmiAiVectorStore store;
umi_ai_vector_store_init(&store);
status = umi_ai_vector_store_add(&store, &chunk, &embedding);
if (status != UMI_STATUS_OK)
    return status;
~~~

Addition checks the complete active store, incoming records and available capacity before publishing a new slot and count. Invalid incoming records return INVALID_ARGUMENT. Malformed stored state returns INVALID_STATE. A valid full store returns CAPACITY_EXCEEDED.

All refusals preserve every byte of the store. Inputs may borrow from an existing or unused entry. Duplicate chunks and mixed embedding dimensions remain allowed; this service does not select a model or enforce a document-identity policy.

## 4. Read stable top matches

~~~c
size_t indices[4];
double scores[4];
if (!umi_ai_vector_store_valid(&store))
    return UMI_STATUS_INVALID_STATE;
size_t count = umi_ai_vector_store_search(
    &store, &embedding, indices, scores, 4U);
for (size_t index = 0U; index < count; ++index) {
    const UmiAiChunk *matched = &store.entries[indices[index]].chunk;
    /* Consume matched and scores[index] while the store remains unchanged. */
    (void)matched;
}
~~~

Search admits every active entry before writing results, so a malformed later row cannot leave apparently valid earlier output. Results are descending by score; ties retain insertion order. Unequal dimensions and zero-magnitude vectors retain the previous neutral-score behavior.

A zero output capacity does no work. Invalid records return zero and leave both output arrays untouched. An empty valid store also returns zero, so the count is not a detailed error status. Use the validators when your application needs to distinguish these conditions.

Output arrays must each be writable for the supplied capacity and must not overlap each other, the query or the store. Callers must synchronize concurrent store mutation and must not retain a borrowed result across changes without owning a copy.

## 5. Qualify the boundary

Run each command separately and stop on errors:

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --target umicom-ai-vector-admission-test umicom-ai-embedding-tests umicom-ai-retrieval-tests umicom-ai-hybrid-retrieval-tests
ctest --test-dir build/windows-ucrt64-debug --output-on-failure -R "^framework[.](ai[.](vector_admission|hybrid_retrieval)|ai_embedding|ai_retrieval)$"
~~~

The authored regression covers malformed counts and text, non-finite components, constructor aliases, offset overflow, full stores, stable ties, neutral scores and untouched refusal outputs. Existing hybrid retrieval cases retain their manually constructed chunks.

Compiler, test, installed-consumer and application qualification remain unexecuted for this change. These checks do not validate a learned model, retrieval quality, network providers or concurrent application lifetimes.
