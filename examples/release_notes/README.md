# Umicom Notes release laboratory

Sammy Hegab · Umicom Foundation · MIT

This is a small C23 Windows example for learning packaging boundaries, not a
replacement for Umicom's document/editor components. It has one text editor,
a real model DLL, a neighbouring UTF-8 starter document and embedded branding.
It counts ASCII whitespace-separated words and saves a new UTF-8 file without
overwriting an existing file. Its input limit is 32 KiB after UTF-8 conversion.

Use the public lesson `docs/learning/check-a-windows-release.html` in Framework.
After configuring the full Applications repository, build:

```powershell
cmake --build --preset windows-ucrt64-all-debug --target umicom-release-notes --parallel 2
```

Open `bin/umicom-release-notes.exe` beside `umicom-release-notes-model.dll` and
`example-note.txt`. Edit, count and save a new copy. The original starter is
loaded again on restart; there is no autosave or multi-document session.

`--smoke-test` has no GUI and returns zero only after loading the real DLL and
reading a nonempty valid starter. The Windows acceptance tests package and
install these actual files to a new Unicode/spaced temporary folder, restrict
the child's PATH, use an unrelated working folder, and separately exercise
missing DLL and missing starter copies. Those tests never start a financial
application or mutate an existing installation.

On Linux only the portable word-count model and the separate PE format tests
can be exercised. That is not a claim that this Win32 example compiled or ran.
The source delivery's Validation.txt distinguishes those results.

The Win32 file calls here are intentionally visible teaching steps. Production
application persistence continues to belong to the existing Framework document
and Data Server services, with their stronger application/session contracts.
