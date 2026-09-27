# Batch 32 — Native Designer risk review

## Reproduced defects fixed

The original canonical document remover erased intermediate nodes before recursing, then could not find them to discover grandchildren. A root/child/grandchild plus unrelated branch fixture retained a grandchild after removing the root. The corrected mark-then-compact implementation removes the complete subtree and keeps unrelated order.

The original node setter passed an existing attribute directly to an initializer that clears its destination before parsing. An invalid integer edit therefore changed the previous property despite returning a parse error. The new setter constructs a candidate first and commits only after success. Alias inputs and fixed-count boundaries are regression-tested.

The lexer accepted unclosed quotes. This is now a parse error; the new export workflow refuses the incomplete model without creating an output directory. Existing valid quotation/comment behaviour remains tested.

## Deliberately unqualified areas

- Windows compiler/SDK integration, UTF-8 argv delivery, wide-path filesystem behaviour, filesystem permissions and installed runtime deployment were not executed on Windows.
- GTK adapter compilation and execution, lifecycle, focus, high-DPI behaviour, accessibility and visual layout have six supplied but unexecuted tests. The headless constructor does not certify them.
- The complete Framework and Applications suite was not built here. Existing authoring history/Undo/Redo, serializers and legacy exporters remain separate integration concerns. Use the full local suite before publication.
- The POSIX tests establish exercised file and allocation behaviour, not sudden-power-loss recovery or a complete security audit. A hostile actor able to replace ancestors can still violate the trusted-directory assumption. Do not run privileged exports into shared writable directories.
- The last completion marker is not a signature or hash. Final directory flush can fail after marker creation; trust the returned result, retained evidence and independent checks, not the marker alone.
- Captions can contain private draft text. Generated source and incomplete export files are plaintext. Review them before committing or sharing.
- This native profile intentionally supports only eight basic component types and text.count. It does not enable arbitrary commands, saving, persistence, a trading/banking workflow, complete catalogue export, source synchronisation, visual palette integration or a packaged release.

## Ownership and bounds

Document operations and creation use the model owner's thread. Plan views borrow memory; caller lifetime must exclude concurrent destruction. GTK references obey the explicit destroy/unref contract. The service checks input limits and cleans its owned buffers on tested allocation failures, but cannot make an invalid pointer or object lifetime safe. A clean sanitizer run is evidence for executed code paths only, not proof that all Umicom products are leak-free.

## Acceptance before release

Build and test the full Windows preset. Run the six GTK lifecycle cases with a display. Build a generated GTK Notes project, exercise Unicode counts and draft isolation, resize it, close it and reopen it. Inspect existing Desk/Studio authoring functionality. Qualify a relocatable runtime package separately before distributing an executable. Keep physical-media writes disabled; this batch does not close earlier boot-media qualification work.
