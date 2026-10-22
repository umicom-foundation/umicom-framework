# Native AdamW Training Publication Markers

A reproducible training experiment has more state than a model checkpoint.
Umicom Framework preserves the decoder weights and AdamW optimiser moments in
a combined checkpoint. A source-locked experiment can also publish three
companion records, each carrying a different set of reviewed facts:

| File | Responsibility |
| --- | --- |
| `checkpoint.umi` | Decoder parameters, step counter and AdamW moments |
| `checkpoint.umi.clip-policy` | Reviewed global gradient-clipping and batch policy |
| `checkpoint.umi.training-lock` | Training settings, learning-rate horizon, seeds and dataset metadata |
| `checkpoint.umi.source-lock` | SHA-256 identity of exact imported source snapshot |

## What is new

A fifth, **optional** file, `checkpoint.umi.publication`, records the exact
byte lengths and SHA-256 digests of those four existing files. It is published
last, after each required input is present and can be read completely.

The record uses a compact, strict ASCII grammar:

```text
UMICOM-AI-TRAINING-PUBLICATION-1
checkpoint_bytes=<decimal length>
checkpoint_sha256=<64 lowercase hex characters>
clip_policy_bytes=<decimal length>
clip_policy_sha256=<64 lowercase hex characters>
training_lock_bytes=<decimal length>
training_lock_sha256=<64 lowercase hex characters>
source_lock_bytes=<decimal length>
source_lock_sha256=<64 lowercase hex characters>
```

The publication service hashes with the **existing Framework SHA-256** implementation,
using a fixed 4096-byte read buffer. It accepts at most 8 MiB for a combined
AdamW checkpoint and 4096 bytes for each individual companion record. Files
must not be empty. A rejected inspection does not overwrite caller-owned
result structures.

The Framework creates the record in an exclusive same-directory staging file
and publishes it under the final name only if that name does not already
exist. An interrupted operation may leave a `.publication.partial` file;
investigate such files instead of automatically deleting them or treating
them as valid evidence. Previously published files are never overwritten.
A multi-file checkpoint bundle is **not** one atomic transaction.

## Read-only verification

After review and a successful native build, the independent executable can
inspect the bundle without creating a decoder or starting training:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$exe = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-publication-verify.exe'
& $exe 'C:\umicom\_local\models\reviewed-final.umi'
```

The native service APIs are:

```c
UmiStatus UmiAiTrainingPublicationSaveNewFile(const char *checkpointPath);
UmiStatus UmiAiTrainingPublicationVerifyFile(const char *checkpointPath);
UmiStatus UmiAiTrainingPublicationVerifyAndInspectFile(
    const char *checkpointPath, UmiAiTrainingPublicationEvidence *outEvidence);
UmiStatus UmiAiTrainingPublicationInspect(
    const char *checkpointPath,
    UmiAiTrainingPublicationEvidence *outEvidence);
```

The educational clipped AdamW executable adds the opt-in `--publish-evidence`
trailing argument. It **implies** strict source-lock and strict run-lock checks,
requires a new output checkpoint path, and on resume first verifies the incoming
publication marker before the existing semantic policy checks. The old CLI
flags and original combined checkpoint bytes remain unchanged.

## Integrity is not authorisation

An offline publication check establishes only that these four **supplied
files** still match the accompanying **supplied record**. Anyone able to
replace the entire unsigned bundle can replace every digest as well. It does
not independently interpret training settings, compare new source files,
validate checkpoint contents through the checkpoint codec, attest machine
security, or verify a digital signature. Before resuming training, the existing
Framework checks must separately compare the approved plan, immutable dataset,
AdamW state, clipping policy, full training lock and source lock.

The publication marker helps distinguish a completed five-file publication
from a partial write after interruption. The operating system's durability
and directory-sync behaviour still matter; this API does not promise
power-loss-atomic publication of all five files. For verifiable authorship or
audit-grade provenance, use a separately reviewed signing and verification
workflow with externally protected keys.

## Exercises

1. Create a disposable bundle with all four files and publish a marker last.
   Explain why the marker must not be created before the source lock exists.
2. Change one byte in the clipping companion. Re-verify and record the status.
3. Explain why replacing all five files can defeat an unsigned digest check.
4. Compare an interrupted `.publication.partial` with a successfully published
   `.publication`. Why must software not silently recover the former?
5. Review the native regression tests for repeat publication, missing files,
   oversized records and output-buffer preservation.
