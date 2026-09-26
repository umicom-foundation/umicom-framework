/* Umicom Framework | assembly practicum | Sammy Hegab, Umicom Foundation | MIT
 * Canonical lesson content. Generated views borrow these immutable records. */
    {
        "assembly.bits",
        "assembly",
        "foundations.bits",
        "1. Quantities as bits and bytes",
        "A computer can represent a workshop stock count as a binary integer. Decimal 12 and hexadecimal 0x0C are two written forms of the same value. The value does not change merely because a debugger displays it in another base.\n\nThe Assembly project totals an array of uint32_t quantities. That type has 32 value bits. This lesson deliberately defines its total modulo 2 to the power of 32: a result that exceeds the largest value wraps around. The C reference and Assembly routine follow the same stated rule.\n\nThis is a lesson about machine arithmetic, not a recommendation for financial totals. A banking balance or stock-control service may instead require checked addition and an explicit overflow error. The appropriate behaviour belongs in the contract, not in an accidental side effect of the chosen instruction.\n\nBefore reading the Assembly, calculate a small total yourself. Use 12 notebooks, 8 pencils and 4 folders. The expected value is 24. You now have an independent expectation rather than merely trusting two implementations that could share the same mistake.",
        "uint32_t quantities[] = {12U, 8U, 4U};\n/* Expected ordinary total: 24. The lesson contract uses modulo 2^32. */",
        "Export the assembly project. Read stock_total.h and write down its overflow policy. Convert 12 to hexadecimal, then predict the total of the three supplied quantities.",
        "Hexadecimal C represents decimal 12. The independent small total is 12 + 8 + 4.",
        "assembly",
        UMI_TEACHER_LANGUAGE_ASSEMBLY, 25U, {
            {"What decimal value is 0x0C?", {"10", "12", "16"}, 1U, "Hexadecimal C is decimal 12."},
            {"What is 12 + 8 + 4?", {"24", "20", "32"}, 0U, "Add the supplied quantities independently before trusting a program."},
            {"Is wrapping arithmetic automatically suitable for money?", {"Yes", "Only on Windows", "No; choose and enforce an explicit contract"}, 2U, "Financial amounts require a deliberate overflow policy, usually rejection rather than accidental wraparound."},
        }
    },
    {
        "assembly.registers",
        "assembly",
        "foundations.assembly",
        "2. Follow the calling convention",
        "A C caller and an Assembly function must agree on where arguments arrive and where the result is returned. That agreement is part of the platform’s calling convention. The same processor can use different conventions on different operating systems.\n\nFor this small x86-64 routine, the Windows branch receives the array pointer in RCX and the count in RDX. The System V branch used by this Linux build receives them in RDI and RSI. Both branches return the 32-bit total in EAX, the low part of RAX.\n\nRead the conditional source rather than copying one register list into every platform. The routine uses only registers permitted to change across the call, makes no nested calls and does not allocate stack storage. Larger routines have additional preservation, alignment and unwind obligations.\n\nOn a platform without the supported Assembly build, the project explicitly selects its C reference and labels that choice. A C fallback is useful for portability, but it is not evidence that the Assembly implementation ran.",
        "#ifdef _WIN32\n    /* array pointer: RCX; count: RDX */\n#else\n    /* System V x86-64: array pointer RDI; count RSI */\n#endif\n    /* result: EAX */",
        "Open stock_total.S beside stock_total.c in Studio. Find the platform branch. Build the project and read whether the test output says Assembly or C reference.",
        "Check the operating-system ABI as well as the processor. A fallback must be labelled honestly.",
        "assembly",
        UMI_TEACHER_LANGUAGE_ASSEMBLY, 35U, {
            {"Where does the Windows x64 first pointer argument arrive here?", {"RCX", "RDI", "RAX"}, 0U, "The Windows x64 first integer or pointer parameter uses RCX."},
            {"Where is this function’s 32-bit result returned?", {"RSP", "EAX", "RDX"}, 1U, "EAX is the low 32-bit part of the return register RAX."},
            {"A build selects the C fallback. Has it tested the Assembly path?", {"Yes", "Sometimes", "No"}, 2U, "The output must distinguish the C reference from the native Assembly path."},
        }
    },
    {
        "assembly.loop",
        "assembly",
        "foundations.assembly",
        "3. Trace the loop before changing it",
        "An Assembly loop is a series of small changes to machine state. This routine clears the total, checks the number of remaining entries, adds one quantity, advances the pointer by four bytes and decreases the remaining count.\n\nThe first count check matters. A caller may supply a null pointer when the count is zero, because the function will not read any element. Removing the check would turn that documented empty-input case into an invalid memory access.\n\nThe pointer advances by four bytes because each uint32_t occupies four eight-bit bytes on the supported target. It does not advance by one whole arbitrary object. A mismatch between element size and pointer step would make the routine read the wrong memory.\n\nTrace the state for two quantities, 12 and 8. After the first iteration the total is 12 and one entry remains. After the second, the total is 20 and none remain. The routine returns instead of reading a third element.",
        "/* Conceptual C equivalent of the Assembly loop. */\nuint32_t total = 0;\nfor (size_t i = 0; i < count; ++i) total += quantities[i];",
        "Use the two-entry example to write a trace table: iteration, total and remaining count. Add a zero-count test without removing the existing one, then explain why the pointer is not read.",
        "Check count before the first read. One uint32_t element advances the pointer by four bytes on x86-64.",
        "assembly",
        UMI_TEACHER_LANGUAGE_ASSEMBLY, 35U, {
            {"What should a zero-count call return?", {"An error string", "Zero without reading the pointer", "The first array value"}, 1U, "The empty total is zero and no element is read."},
            {"How far does the pointer advance per uint32_t element here?", {"One byte", "Eight bytes", "Four bytes"}, 2U, "The supported x86-64 target stores this 32-bit element in four bytes."},
            {"After adding 12 and 8, what is the total?", {"20", "12", "8"}, 0U, "Trace the two additions independently: 0 + 12 + 8 = 20."},
        }
    },
    {
        "assembly.compare",
        "assembly",
        "foundations.c-modules",
        "4. Compare independent expectations",
        "Testing Assembly requires more than checking that the linker found a symbol. The returned values must satisfy the contract, including empty input, ordinary quantities and the deliberate modulo boundary.\n\nThe project keeps a readable C reference and compares it with the selected implementation. It also checks hand-calculated examples, because two implementations can repeat the same mistake. A deterministic generated sequence supplies more varied arrays without making test results depend on chance.\n\nRun the tests before editing the routine. Save a copy of your explanation, change one operation, and observe which expectation fails. Restore or correct your change without deleting the assertion. Compiler diagnostics and failed tests are feedback, not obstacles to hide.\n\nDo not infer a performance improvement simply because a function is written in Assembly. Establish correctness first. A meaningful performance comparison would need representative inputs, an appropriate measurement method and the same build settings. This project does not claim a speed advantage.",
        "/* Both a direct expectation and a reference comparison matter. */\nCHECK(StockTotal(values, 3) == 24U);\nCHECK(StockTotal(values, 3) == StockTotalC(values, 3));",
        "Run the comparison tests. Explain the difference between a hand-calculated expected value and comparing two implementations. Keep the test output beside your source changes in Studio.",
        "Two agreeing implementations can still be wrong. Include known answers that were calculated separately.",
        "assembly",
        UMI_TEACHER_LANGUAGE_ASSEMBLY, 40U, {
            {"Why include a hand-calculated expected total?", {"To catch a mistake shared by both implementations", "To avoid running tests", "To change the ABI"}, 0U, "Independent expectations catch errors that implementation-to-implementation comparisons can miss."},
            {"Does Assembly automatically prove an implementation is faster?", {"Yes", "No", "Only with small arrays"}, 1U, "Performance requires measurement; the language alone proves nothing."},
            {"What should happen to a useful failing assertion?", {"Delete it", "Ignore it", "Keep it and investigate the violated contract"}, 2U, "A regression test preserves the behaviour the implementation must satisfy."},
        }
    },
