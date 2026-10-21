# Umicom Native AI: reviewed C23 teaching corpus

**Umicom Foundation · Sammy Hegab · MIT**

The files `train/account.c`, `train/notes.c` and `eval/payment.c` are deliberately
small, original fictional C23 examples. They contain no live financial records,
private SDK APIs or secrets. They are for measuring a minimal byte-level model's
ability to update parameters and evaluate on a held-out **file**.

The manifest `manifest.txt` selects two training files and a different
held-out evaluation file. Each import has a documented partition and a
noncryptographic FNV-1a64 provenance fingerprint over its captured bytes.
The checksum does **not** prove authenticity and a whole-file split does **not**
eliminate duplicates, related files or copied code across partitions.

The dataset manifest is a reviewed local-input format, not a sandbox. Source
paths can traverse symlinks or junctions outside their directory. Do not import
unreviewed manifests or confidential/third-party data without permission.

All reusable parsing, source-file ingestion, tokenisation, seeded sampling and
training are implemented by Umicom Framework. The executable in
`../train_source.c` is only an educational consumer.

Example from the `Umicom-Applications` root:

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-source-train.exe" `
  ".\framework\learning\native-llm\fixtures\manifest.txt" 64
```

A changing loss is not evidence that the model understands banking or C23.
The files are too small to support claims about generalisation.
