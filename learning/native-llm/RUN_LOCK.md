# Reproducible AdamW continuation: a dataset and schedule run lock

A checkpoint can reproduce model weights and AdamW moments without proving that the *next* training run will apply the same optimisation policy. In particular, two identical checkpoints may diverge if they use different training files, file order, initialisation or sampling seeds, learning-rate schedules, microbatch partitions or clipping thresholds.

Umicom Framework provides an opt-in, native C23 **run lock** to detect many of these mismatches before continuation. The original combined checkpoint and `.clip-policy` formats remain unchanged. A strict run creates one additional `*.training-lock` companion alongside them.

## What the run lock records

```text
UMICOM-AI-ADAMW-RUN-LOCK-1
checkpoint_sha256=<64 lowercase hexadecimal characters>
dataset_snapshot_sha256=<64 lowercase hexadecimal characters>
run_settings_sha256=<64 lowercase hexadecimal characters>
```

The checkpoint digest covers the exact byte sequence passed to the C23 service. The native laboratory passes the canonical combined checkpoint encoding. The dataset digest covers an *ordered catalogue* of the immutable corpus snapshot: partition, relative path, byte length and existing FNV-1a64 source fingerprint for every document. It detects ordinary modifications or reordered files, but **FNV-1a64 is not collision-resistant**. SHA-256 applied to that metadata cannot turn its embedded FNV fingerprints into cryptographic source authentication.

The run-settings digest includes the decoder's dimensions and model seed; train/evaluation seeds; evaluation interval and batch sizes; AdamW beta parameters, epsilon and weight decay; the learning-rate schedule **kind, peak and minimum, warm-up and entire absolute horizon**; plus the gradient clip threshold and batch/microbatch partition. Float values are hashed as canonical 32-bit representations. The only excluded plan field is `steps`, the requested current stopping point. This allows a four-step checkpoint to continue through eight steps under the same original eight-step schedule.

## Two compatible modes

The existing clipped command continues to accept its established arguments and `.clip-policy` record. The additional final `--strict-run-lock` argument enables the stronger checks **explicitly**. A strict run publishes both the original `.clip-policy` and the new `.training-lock`. A strict resume refuses either record being absent or mismatched; it never invents historical evidence for an older checkpoint. The existing un-clipped AdamW CLI remains unchanged.

On a reviewed Windows development machine, after building and receiving security clearance for the binary:

```powershell
Set-Location 'C:\umicom\Umicom-Applications'
$exe = '.\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-clipped-experiment.exe'
$first = '.\framework\learning\native-llm\fixtures\clipped-adamw-experiment.conf'
$continuation = '.\framework\learning\native-llm\fixtures\clipped-adamw-continuation.conf'

& $exe $first 8 'C:\umicom\_local\models\first-locked.umi' --strict-run-lock
& $exe $continuation 8 'C:\umicom\_local\models\continued-locked.umi' 'C:\umicom\_local\models\first-locked.umi' --strict-run-lock
```

Use an existing reviewed output directory and *new* destination names. Never overwrite a previous result. The original cosine schedule total of **eight** steps must not change between the two invocations.

## Inspect the C23 implementation

The public API is declared in `include/umicom/ai/training/training_run_lock.h`. Its implementation is `src/ai/training/training_run_lock.c`; it reuses the Framework SHA-256 service and the approved dataset catalogue. `UmiAiTrainingRunLockFormat` produces a deterministic bounded text record; `UmiAiTrainingRunLockVerify` checks that exact representation; `SaveNewFile` refuses to overwrite an existing record. Failed format calls never partially change caller buffers.

A successful lock verification means the submitted checkpoint, dataset metadata and policy match the recorded values. It does **not** establish who produced the record, that the source is authorised, that a hostile actor could not engineer a fingerprint collision, or that the operating system and compiler were trustworthy. Software signatures, source approval, independent hashes of approved original files and release security review remain separate checks.

## Reproduce the regression tests

CTest registers three focused cases: strict run-lock identity/atomicity, exclusive record-file publication, and a real on-disk combined AdamW checkpoint resumed to byte-identical final model and moments. The on-disk case also rejects reordered dataset sources, changed schedule horizon and missing run lock, then confirms that the borrowed checkpoint was not mutated. These checks require the **actual native Framework services**. A syntactically correct isolated component build with substituted dependencies is not a substitute for these integration tests.
