![Umicom](../../resources/brand/umicom-logo.svg)

<!-- Corrected the draft ../assets/branding/umicom-logo.svg reference to the shared Framework brand master above. -->

# Inspect training and evaluation overlap before fitting a model

Training examples influence a model's parameters. Evaluation examples are held
aside so that you can measure behavior on material not used in that fitting step.
Copying the same prompt and answer into both sides can make evaluation look
better than it really is. A different filename or source ID does not make copied
content independent.

Framework provides UmiAiTrainingJsonlAuditOverlap in
umicom/ai/training/jsonl_overlap.h through Umicom::ai. It reads the existing
source_id, prompt and completion JSONL teaching schema and an already reviewed
membership array. Zero means training; one means evaluation. This is an audit,
not a splitter, tokenizer, trainer, vector database or automatic deduplicator.

## Follow two identical pairs

~~~json
{"source_id":"original","prompt":"What stores working values?","completion":"RAM."}
{"source_id":"copied-note","prompt":"What stores working values?","completion":"RAM."}
~~~

With membership {0,1}, both rows form one distinct pair. There is one duplicate
row beyond the first occurrence, one shared pair across sides, one affected
training row and one affected evaluation row. The original one-based witness
lines are 1 and 2. The operation returns OK because the inspection completed;
sharedPairs being nonzero is the observation your workflow should review.

The comparison excludes source_id and compares decoded prompt and completion
as two separate strings. JSON escape variants match. Whitespace, case and Unicode
normalization variants remain distinct. Prompt ab plus completion c is different
from prompt a plus completion bc. There is no probabilistic hash equality claim.
Near-duplicate text, paraphrases, related document chunks, duplicated images,
video frames and material inside an existing model's pretraining are outside
this exact text-pair check.

## Use the shared operation

~~~c
UmiAiTrainingJsonlOverlapSummary overlap;
UmiAiTrainingJsonlReport report;
const unsigned char membership[] = {0, 1};
UmiStatus status = UmiAiTrainingJsonlAuditOverlap(
    jsonl, jsonlBytes, membership, 2, 4096, NULL, &overlap, &report);
if (status == UMI_STATUS_OK && overlap.sharedPairs != 0) {
    /* Present original witness lines for review before fitting a model. */
}
~~~

The snippet assumes jsonl points to the two records above and jsonlBytes is the
exact byte count excluding a C string terminator. A null cancellation token means
no cancellation request. The membership count must match the actual row count,
not its buffer capacity. Keep it paired with the same input bytes and row order.

| Summary field | Meaning |
| --- | --- |
| rows | Number of validated records |
| trainingRows / evaluationRows | Rows assigned to each side |
| distinctPairs | Number of unique decoded prompt/completion pairs |
| duplicateRows | rows minus distinctPairs, including repetitions within one side |
| sharedPairs | Distinct pairs with at least one row on each side |
| trainingRowsInSharedPairs / evaluationRowsInSharedPairs | All affected rows, not just one witness |
| firstTrainingLine / firstEvaluationLine | Lowest training witness across shared pairs and lowest evaluation witness for that same pair; zero if none |
| stagingBytes | Owned text and row descriptor payload used by this call |

Repeated rows on only one side increase duplicateRows but do not increase
sharedPairs. An empty side is accepted by this inspection API; it does not mean
that an empty evaluation set is suitable for your training workflow.

## Bound memory and preserve failed outputs

1. Validate every record and measure decoded text using the existing JSONL reader.
2. Check the exact membership count and the sum of text plus descriptor bytes.
3. Refuse if that sum exceeds maxStagingBytes, before allocating the audit's staging.
4. Allocate the exact measured text and descriptor payloads, then scan again to
   copy the borrowed callback text into those owned buffers.
5. Sort the private descriptors, compare exact pairs, and count original-line witnesses.
6. Publish the complete summary only after every stage and final cancellation check.
7. Clear the private text and descriptors before releasing them.

The explicit budget excludes parser temporary allocations, stack, allocator
overhead and caller-owned buffers. It is not a whole-process memory ceiling.
The input is limited to 16 MiB and 4096 rows by the existing reader. Pointer width
can change the descriptor payload and therefore stagingBytes. Sorting is bounded
but not interruptible within qsort; cancellation is observed around sorting and
during grouping. Applications should run substantial audits in their existing
worker service rather than blocking an interactive UI.

Summary storage stays unchanged on every failure. A nonnull report is initialized
on entry and describes the latest scan, so COMPLETE may accompany a later budget,
allocation or membership-count refusal. Check status first. Never interpret an old
summary as a new success. Input, membership and outputs must be valid separate
objects, with no concurrent mutation during the synchronous call.

## Build the existing SDK example

Use a matching installed SDK with the ai component. Substitute your actual SDK
prefix for the placeholder, and stop on errors. This exercise does not need a
Framework checkout inside its source folder.

~~~powershell
cmake -S . -B build -G Ninja -DUMICOM_SDK_ROOT="C:/path/to/installed-umicom-sdk"
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure --no-tests=error
.\build\bin\umicom-sdk-training-data.exe
~~~

Run these commands from the SDK's shared examples/training_data folder or a copy
of that complete example. Its CMake file checks the selected installed SDK and
applies its shared branding helpers. Missing jsonl_overlap.h or an unresolved
function means the chosen SDK needs the matching implementation. Do not hide
that mismatch by copying private implementation files into the consumer.

The example first shows a clean four-row split and then deliberately reports the
copied pair above. Source-focused checks cover exact/escaped identity, field
boundaries, witnesses, repeated rows, exact and insufficient budgets, cancellation,
row limits and every injected parser/staging allocation failure. An installed
SDK run and application integration require their own qualification.

## Correct the data deliberately

Review a reported pair's provenance and purpose. Remove a redundant example only
through your reviewed data-editing workflow, or keep a family of related examples
on one side. Record the updated dataset version and membership. Regenerate an
evaluation result after any change. Exact non-overlap is one useful check among
rights review, privacy review, source grouping, near-duplicate inspection and a
real held-out evaluation; it cannot replace them.

See [the SDK example](../../examples/training_data/README.md) and
[byte-pair preparation](byte-pair-preparation.md) for the later tokenizer boundary.
