![Umicom](../../resources/brand/umicom-logo.svg)

# Review a dataset before deciding whether to train

A source group identifies records derived from the same original document or
other reviewed source. A training partition supplies examples used to change
model parameters. An evaluation partition supplies examples kept apart for
assessment. Moving rows at random without respecting their origin can put
related material on both sides.

Grouping is necessary, but not sufficient. Two different source names can hold
exactly the same prompt and completion. Framework's review operation composes
its existing source-group splitter and exact-pair overlap audit, using the same
input bytes for both. Successful review means the inspection completed. It can
report shared pairs and does not approve a dataset.

## Run the native command

1. Prepare a UTF-8 JSONL file with the strict source_id, prompt and completion
   schema from the earlier training-data guide. One object occupies each line.
2. Choose a positive number of complete source groups for evaluation. Leave at
   least one actual group for training. A group count is not a row percentage.
3. Choose an unsigned decimal seed and record it with the reviewed dataset.
4. Choose a positive overlap staging budget in bytes. The example budget is
   65,536 bytes; this is not a limit on total process memory.
5. Run the command with an absolute file path. Use your actual native umicom
   executable or its explicitly verified path.

~~~powershell
umicom training-data review C:\datasets\lesson-pairs.jsonl 1 42 65536
~~~

The output reports source/row counts, distinct pairs, duplicate rows, shared
pairs, original witness line numbers and overlap staging bytes. It does not
print source IDs, prompt/completion content, membership bytes or the selected
path. A shared pair has the same decoded prompt AND completion on both sides.
Source IDs do not participate in that content comparison.

Exit zero means the review completed, including when shared pairs were found.
Exit one means the input or operation was refused; exit two means command usage
was invalid. A pipeline must inspect the review result and apply a separately
chosen policy. Do not treat exit zero as a release, rights or model-quality gate.

## Reuse the SDK service

Include umicom/ai/training/jsonl_review.h for an immutable memory snapshot, or
umicom/ai/training/jsonl_file.h for one selected file. Link the exported AI
component. The file adapter reuses Framework's regular-file owner and clears
its acquired input buffer before release.

~~~c
UmiAiTrainingJsonlReviewOptions options = {1U, 42U, 65536U};
unsigned char membership[UMI_AI_TRAINING_JSONL_ROWS];
UmiAiTrainingJsonlReviewSummary summary;
UmiAiTrainingJsonlReport report;
UmiStatus status = UmiAiTrainingJsonlReview(
    bytes, byte_count, &options, NULL,
    membership, sizeof membership, &summary, &report);
if (status == UMI_STATUS_OK) {
    /* Inspect summary.overlap.sharedPairs before choosing a data policy. */
    /* Only summary.partition.rows membership bytes are valid. */
}
~~~

bytes and byte_count describe the caller's unchanged owned input. Each successful
membership byte is zero for training or one for evaluation. The existing
JsonlSelectionWrite service can serialize a reviewed side from those SAME
bytes; review itself creates no file. Inputs and outputs must be separate valid
objects. Keep all borrowed storage alive and unchanged until the call returns.

The file adapter reads once and reviews that acquired buffer. It returns neither
a file digest nor the buffer itself. A membership array from that call must not
be applied blindly to a file reopened later: another writer may have reordered
or replaced its rows. For an export workflow, own a snapshot and use the memory
review and selection services on it, with a separate publication policy.

## Understand failure and memory

Membership and summary remain unchanged if parsing, splitting, capacity checks,
allocation, overlap auditing or cooperative cancellation fail. The report is
progress evidence for the latest parser pass. COMPLETE can accompany a later
staging-budget failure; it is not the whole operation's success result. Always
check the returned status before using result buffers.

The maximum source is 16 MiB and 4096 rows. The file path reads the full bounded
source plus a terminating byte; it is not the streaming visitor. Review owns a
fixed 4096-byte candidate membership array and invokes the shared splitter and
auditor sequentially. maxStagingBytes limits the auditor's owned text and row
descriptors only. Parser allocation, splitter storage, input bytes, stack,
allocator overhead and caller storage are outside that budget. Clearing owned
buffers does not erase OS caches or the original file.

The operations are synchronous. Use an application worker with a valid
cancellation token and an explicit result handoff. Cancellation checks do not
interrupt a blocking native read or every step inside a bounded sort. Do not
free the input or close its owning application state while the worker borrows it.

## Limits of the observation

Exact comparison decodes JSON escapes, but does not normalize case, whitespace
or Unicode normalization forms. It does not find paraphrases, prove source rights,
check label correctness, identify future-data leakage, or validate a tokenizer.
A source-group split is not a chronological or stratified holdout. Adding a new
source group can change the assignment for the same seed. Save the reviewed
input identity and assignment when implementing a publication workflow.

The complete SDK teaching program in examples/training_data demonstrates both
separate services and the composed operation. Its source can be checked against
current production objects; qualification of an installed SDK remains a separate
step from that source-level exercise.
