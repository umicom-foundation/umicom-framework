# Review a local banking command

Build this directory to exercise the actual Framework banking service with a small canonical dependency subset. The complete lesson is `lesson.c`; `main.c` invokes it with `demo` (full prediction text) or `--self-test` (checked concise results). It uses only memory storage and fictional actors, accounts and money.

```sh
cmake -S framework/examples/bank_review -B build/bank-review -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/bank-review --parallel 2
ctest --test-dir build/bank-review --parallel 2 --no-tests=error --output-on-failure
build/bank-review/bin/umicom-bank-review demo
```

Use `-DUMICOM_BANK_REVIEW_SQLITE=OFF` to test unavailable persistent storage. SQLite-dependent tests report skipped, not passed. `-DUMICOM_BANK_REVIEW_SANITIZE=ON` instruments the native targets with AddressSanitizer/UndefinedBehaviorSanitizer on supported GCC/Clang hosts. `-DUMICOM_BANK_REVIEW_GTK4=ON` requires actual GTK4 >=4.10 development files and compiles the real adapter; its lifecycle tests need a display.

Install the focused SDK into a separate prefix, then use `find_package(UmicomBankReview CONFIG REQUIRED)` and link `Umicom::bank_operations`. The normal complete Framework SDK exports that same banking target. Do not combine the focused and full SDK targets in one project.

Read `../../docs/learning/bank-review.html` for the public lesson and `../../docs/development/bank-review.md` for ownership and integrity boundaries. No real payments, account login or network operations are implemented by this example.

The complete separate consumer is in `installed_client/`. After installing the focused build, configure that directory with `-DCMAKE_PREFIX_PATH=<your isolated prefix>`, build and run CTest. Its only source includes public installed headers and verifies that review alone creates no customer.
