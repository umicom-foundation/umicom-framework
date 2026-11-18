![Umicom](../../resources/brand/umicom-logo.svg)

# Inspect training examples with the Framework SDK

This small C23 program reads two original prompt/completion records from a string.
It then deliberately rejects a number where a completion string was expected.
It does not load a model, train weights, call a provider or read your documents.

A **record** is one example. A **schema** describes its permitted fields.
A **callback** is a function the reader calls for each accepted record.
The callback borrows the decoded record only until it returns.

1. Obtain a Framework SDK built with the public header
   `umicom/ai/training/jsonl_pairs.h` and its matching `Umicom::ai` library.
   An older package can have the same overall version without this addition;
   a header alone does not prove its library contains the implementation.
2. Open PowerShell in this example directory. Use a compiler compatible with
   that SDK. For UCRT64, add its tool directory to this process's search path.
3. Replace the example SDK prefix below with your actual installed prefix.
4. Run each command separately and stop if it fails.

~~~powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S . -B build -G Ninja -DUMICOM_SDK_ROOT=C:/SDK/Umicom
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
~~~

The success path reports two accepted rows. The second exercise reports that line
2 was rejected after one accepted callback. Count **bytes** separately from
model **tokens**: UTF-8 text and a model-specific tokenizer have different units.

The parser accepts exactly `source_id`, `prompt`, and `completion`. Each is a
string. Source identity and completion must be nonempty; an empty prompt is
allowed. Duplicate keys, extra fields and malformed Unicode are refused.
The reader limits each decoded text to 4,096 bytes, each source ID to 128 bytes,
each physical line to 32,768 bytes, and a scan to 4,096 rows / 16 MiB.

The caller owns the input. It must remain unchanged while scanning. The parser
does not retain it. Copy callback values if they are needed after return.
If a later record fails, previous callbacks remain completed. Inspect
`acceptedRows`, `consumedBytes`, `failedLine` and `stage` before deciding whether
to discard a candidate dataset or resume a separate reviewed operation.

This schema is a teaching contract, not universal compatibility with a hosted
service, chat template, tokenizer or trainer. Review data rights and quality,
preserve related-source groups during partitioning, then select a documented
model-specific conversion.

If CMake cannot identify the compiler, check its version and UCRT64 DLL search
path. If the header is missing or linking reports an undefined reader function,
the selected SDK does not contain this addition; select or build a matching
package rather than copying a header over an older library. Installed-SDK and
application qualification are separate from focused source tests.

## Prepare a record without hand-building JSON

The example also fills a UmiAiTrainingPair and calls UmiAiTrainingPairWrite.
A newline inside the answer is escaped inside its JSON string, and one real LF
ends the record. The returned byte count includes that LF and excludes NUL.
Capacity includes NUL. Both output bytes and the count remain unchanged on failure.
The decoded fixed arrays must contain terminators; do not fill them beyond their
limits. Escape expansion can exceed the line limit even when decoded fields fit.
The writer stages and clears a bounded temporary buffer, then copies a complete
record. It never opens, replaces or uploads a dataset file.

## Inspect the dataset before choosing a model

The example also calls `UmiAiTrainingJsonlSummarize` from
`umicom/ai/training/jsonl_summary.h`. It prints two rows, two source groups and a
largest group of one row. It then passes malformed data and verifies that the
previous complete summary remains unchanged.

1. Count the two embedded objects in `main.c`.
2. Find their `source_id` values. Each identifies one independent teaching note.
3. Compare `rows` with `sourceGroups`. Several chunks from one note should retain
   the same source ID, so these numbers need not match.
4. Read `longestPromptBytes` and `longestCompletionBytes` when sizing your data
   buffers. These are decoded UTF-8 byte lengths, **not model token lengths**.
5. Before model training, use the exact tokenizer and formatting contract of the
   selected model and keep related source groups in one partition.

The summary is not a duplicate detector or a data-rights approval. It groups
exact decoded source ID bytes; case and Unicode normalization remain distinct.
Malformed records, capacity failures and cancellation leave the caller's summary
unchanged. The separate report describes how far input parsing progressed.
Bounded temporary source IDs are cleared and freed before return; prompts and
completions are not accumulated into another corpus.

## Inspect one selected local file

Include `umicom/ai/training/jsonl_file.h` and call
`UmiAiTrainingJsonlInspectFile` when the input is an explicitly chosen file.
Pass an ordinary absolute path, a summary and a separate progress report.
The service reuses platform regular-file admission and the same strict JSONL
summary implementation. It limits the file to 16 MiB before reading.

~~~c
UmiAiTrainingJsonlSummary summary;
UmiAiTrainingJsonlReport report;
UmiStatus status = UmiAiTrainingJsonlInspectFile(
    absolute_path, NULL, &summary, &report);
if (status == UMI_STATUS_OK) {
    printf("Rows: %zu; groups: %zu\n", summary.rows, summary.sourceGroups);
} else {
    printf("Input refused; accepted rows: %zu; failed line: %zu\n",
        report.acceptedRows, report.failedLine);
}
~~~

The summary stays unchanged on failure; never use an uninitialized summary after
a failed call. The progress report is initialized even if file admission fails.
The final path component must be an ordinary file; parent aliases may still be
followed. This is not directory confinement. Synchronous file I/O can block, so
interactive applications should call it on a worker. Cancellation is checked
before reading and during parsing, not inside the file read itself.

The owned input returned by a successful read is cleared before release. This
does not erase the original file, filesystem caches or every parser allocation.

The native CLI exposes the same service:

~~~powershell
umicom training-data --help
umicom training-data inspect "C:\umicom-learning\training\corpus.jsonl"
~~~

Use a matching executable that contains the command. It reports aggregates
without printing source IDs, prompts or completions. Exit 0 means the strict
schema was accepted; exit 1 means input was refused; exit 2 means command usage
was wrong. No model is loaded, no training begins and no input file is changed.

A JSON record's raw-byte limit excludes its LF/CRLF delimiter; the whole-input
limit includes delimiters. These are storage limits, not tokenizer limits.

## Keep related records in one partition

The example also calls `UmiAiTrainingJsonlPartition` on four original rows
from three source notes. It selects one source group for evaluation with seed
42. Both rows from note-a receive the same membership byte. The printed row
counts need not have the same ratio as the group counts.

The operation validates the entire input before publishing membership and
summary outputs. It maps exact decoded source IDs to integer ranks and reuses
Framework's group splitter. It does not hash identifiers, infer document
relationships, detect near-duplicates or write dataset files. Preserve your
reviewed source identities and save the resulting assignment with the dataset;
adding a new source can change a seeded assignment.

This additional API requires a matching SDK containing
`umicom/ai/training/jsonl_partition.h` and its AI implementation. A missing
header or unresolved symbol indicates that the selected SDK does not yet
contain this feature. Update and qualify the SDK before retrying; do not copy
private implementation files into an installed consumer to hide the mismatch.

## Serialize the reviewed sides without changing a file

The example now includes `umicom/ai/training/jsonl_selection.h` and calls
`UmiAiTrainingJsonlSelectionWrite` twice with the same original bytes and
membership array. Side zero selects training rows; side one selects evaluation.
It parses both resulting strings and checks that all four rows remain accounted
for. The additional output line reports encoded bytes, not tokens.

1. Keep the original bytes and row order unchanged after partitioning.
2. Supply a separate output buffer and capacity including its final NUL.
3. Check the returned status before reading a new selection summary.
4. Use `outputBytes` when passing the resulting JSONL to another byte-span API.
   This count includes each LF but excludes the C string's final NUL.
5. Treat writing files or adapting records to a trainer as a separate reviewed
   step. This example deliberately keeps both sides in memory.

Every input record is validated, including records not selected. A sizing pass
is followed by an exact-size temporary output allocation and another scan.
Output and summary remain unchanged on any failure. Selected values and order
are preserved, while field order, whitespace and escape spelling are canonicalized.
A selection containing no rows succeeds with an empty string; the next workflow
must decide whether an empty training or evaluation set is acceptable.

The temporary corpus and row buffer are cleared before release. This does not
remove the original data or promise that all library/operating-system copies
are erased. The caller owns its output buffers and their eventual clearing.
Two successful individual calls are not an atomic two-file transaction. A
caller that needs both results must keep them private until both calls succeed.

## Count predictions without confusing them with training

The final exercise calls `UmiAiTrainingPredictionScore` from
`umicom/ai/training/prediction_metrics.h` through `Umicom::ai`.
Use an SDK whose headers **and library** contain this API. The program creates
two rows of three invented scores. The target token ID is zero in both rows.
The first target ranks first; the second ranks second. The output therefore
reports top-one 1/2 and top-2 2/2.
These are arithmetic checks, not results from a trained model.

1. Find the six numbers in `logits`. Each consecutive group of three is one
   row. Its array indices identify candidate tokens, beginning at zero.
2. Compare each target's score with the other scores in that row. Larger scores
   rank first. Equal scores use the lower token ID first, including signed zero.
3. Read `topOneCorrect` together with `selectedRows`. For a nonzero
   denominator, their ratio is the selected top-one accuracy. Aggregate these
   integer counts across uneven batches before calculating a percentage.
4. Follow the second call with selection bytes zero and zero. A successful
   count with no selected rows has no accuracy ratio. The example prints
   “accuracy unavailable” instead of dividing by zero or reporting perfection.
5. Compare this operation with JSONL validation above. Accepting a record checks
   its storage schema. Ranking predictions checks model scores against known
   token IDs. Neither establishes factual correctness or image/video quality.

All scores must be finite and all target IDs valid, even in excluded rows.
A failed call leaves the previous count structure unchanged. The function
borrows stable input arrays only for the duration of the call; it allocates no
score copy and does not run a model. Selection bytes choose evaluated rows,
not attention visibility or training/evaluation dataset membership.

The teaching trainer and exported AI component use this same implementation.
A focused source test does not establish installed-library export, runtime DLL
lookup or end-to-end application qualification; run the installed example with
the selected SDK to verify those boundaries.

## Inspect repeated content across partitions

The example also calls UmiAiTrainingJsonlAuditOverlap through Umicom::ai.
It first checks the four-row source-group exercise: its four distinct pairs
have no exact overlap. It then deliberately places the same question and answer
under two different source IDs, one on each side. The audit reports one shared
pair, with training line 1 and evaluation line 2. A successful audit can report
problematic data; success means the inspection completed, not that training is safe.

1. Keep the reviewed membership array aligned with the original JSONL row order.
2. Set a staging budget. This small example allows 4096 bytes for owned text and
   row descriptors, not for the whole process or its JSON parser temporaries.
3. Check the status before reading a new summary. Failed parsing, allocation,
   budget admission or cancellation leaves that summary unchanged.
4. Read sharedPairs together with trainingRowsInSharedPairs and
   evaluationRowsInSharedPairs. Repeating a pair many times increases affected
   rows, while the number of distinct shared pairs can stay one.
5. Inspect the original one-based witness lines. Keep the reviewed correction
   explicit; this service does not delete records or move them to another split.

Comparison uses decoded prompt/completion bytes, ignoring source_id. JSON escape
variants match, but case, whitespace, Unicode normalization variants and similar
meaning do not. Separate fields remain separate: prompt ab plus completion c
is different from prompt a plus completion bc. No vector database or embedding
model is needed for this exact comparison. No-overlap is not a guarantee against
leakage from paraphrases, adjacent document chunks or model pretraining.

The implementation measures first, allocates exact staging payloads within the
budget, copies borrowed callback text, sorts privately and clears staging before
release. Sorting does not change the input bytes or membership. The exact
stagingBytes value can vary with platform pointer width. Installed SDK exports
and application integration still need their own qualification with matching
headers and libraries. See the [overlap guide](../../docs/learning/training-data-overlap.md).

## Inspect native byte-pair context fit

The second program, `context.c`, uses the shared context inspector. It counts three synthetic records requiring 3, 4 and 10 prediction rows. At context 4, two fit and one is overlong. The service does not truncate or filter them.

1. Use a current matching SDK containing `jsonl_context.h` and its AI library implementation.
2. Configure this project as above.
3. Build `umicom-sdk-training-context`, then run its test.

~~~powershell
cmake --build build --target umicom-sdk-training-context --parallel 2
ctest --test-dir build -R sdk.training-data.context --output-on-failure
& .\build\bin\umicom-sdk-training-context.exe
~~~

Expected counters: records 3, fitting 2, overlong 1, first overlong line 3; required rows 17, fitting valid rows 7, fitting selected rows 6, fitting padding rows 1. These are native byte-token positions, not another model's token counts. The current teaching decoder permits at most 32 context rows. Full parsing and a fit count do not establish memory affordability or training quality.

For a selected local JSONL file, the same inspection is available as:

~~~powershell
umicom training-data context "C:\umicom-learning\training\corpus.jsonl" 4
~~~

Use a matching native CLI. A completed scan can report overlong records and still exit zero; no training approval, truncation or model launch is implied. The context is decimal 1..32 for this byte teaching format. See the [context guide](../../docs/learning/jsonl-context.md) for file ownership, cancellation, output privacy and exit-status details.


## Read one selected file with bounded input retention

The third executable, `umicom-sdk-training-stream`, accepts an absolute JSONL file path. It links AI and Platform so the same native argument owner preserves Unicode paths on Windows. Obtain matching `jsonl_stream.h` and library implementations in the selected SDK; adding a header to an older package is insufficient.

Create a small UTF-8 file without a BOM containing these two lines:

~~~json
{"source_id":"a","prompt":"p","completion":"c"}
{"source_id":"a","prompt":"p","completion":"c"}
~~~

After the normal configuration and build above, run:

~~~powershell
& .\build\bin\umicom-sdk-training-stream.exe C:\Learning\records.jsonl
~~~

Replace the absolute path with your actual file. Expected output is rows: 2, prompt bytes: 2 and completion bytes: 2. Each number is an aggregate, not a model-token count. The program does not load or train a model. Replace the second line with {} and rerun: it must return failure, report one accepted row and failing line 2, and withhold the success counters. Restore your teaching data afterward.

The line reader keeps one bounded line allocation rather than the full corpus. Its callbacks still run as records arrive: they cannot be rolled back by the reader. This consumer therefore accumulates a private candidate and publishes only after the whole visit succeeds. The existing memory examples and file-summary/context services retain their earlier contracts. Read [the streaming guide](../../docs/learning/jsonl-streaming.md) for report stages, cancellation and memory limits.

## Review one snapshot before using a split

The main example also calls `UmiAiTrainingJsonlReview`. It groups the two repeated records by their different source IDs, finds one exact pair across the two sides, and then repeats the call with an insufficient staging budget. That refusal leaves the earlier membership and summary unchanged. The supplied fixture is original teaching text; successful inspection is not dataset approval. See [the review guide](../../docs/learning/dataset-review.md) for the selected-file adapter, native command, memory accounting and snapshot identity limits.
