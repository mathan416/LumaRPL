# Writing LumaRPL help

Write for someone holding the calculator. Tell them what they can do, what to press or type, and what they should see. Read the paragraph aloud: if it sounds like a build report or a description of the implementation, rewrite it.

Use plain sentences. Name the current screen or command when context matters. Put essential controls and prerequisites before the steps. Give a small example with an expected result, and state angle mode when it changes the answer. Explain a limitation when it affects the task; keep it out of unrelated entries.

A command reference can be concise and technical without sounding mechanical. Describe the operation and operand order. Do not prepend classifications, repeated compatibility warnings or generic instructions to every entry. If a command is unavailable, say so and give a working alternative where one exists. Verify that claim against the implementation.

All help uses native controls. Lists use arrows/Enter, visible row numbers and labelled actions. Text pages use Up/Down to scroll, Left/Right to page, and F5/Esc to return. The calculator's six-column soft menus are separate; help must not introduce that navigation model again.

Sources:

- docs/ui/reference.json: the built-in guide and matching computer-readable guide.
- docs/ui/tool-help.json: contextual tool help.
- docs/ui/menu-help.json: menu/action Details.
- docs/help/commands.json, settings.json and flags.json: newly written command-reference entries.
- docs/help/examples.json: executable examples and expected real-engine results.
- tools/generate_native_reference.py: bounded native index/data files and a readable Markdown export.
- db48x/src/ids.tbl and command implementations: accepted spellings and actual behavior.

Run the generators and content tests. Execute examples with the real engine. Exercise the production reader, including missing resources, malformed data, Back, page/selection retention and calculator-state preservation. Inspect actual captures and report their scope. Device readability remains a separate check.

Research behind these rules:

- [NN/G: Help and Documentation](https://www.nngroup.com/articles/help-and-documentation/)
- [NN/G: Reading Content on Mobile Devices](https://www.nngroup.com/articles/mobile-content/)
- [Microsoft: Writing step-by-step instructions](https://learn.microsoft.com/style-guide/procedures-instructions/writing-step-by-step-instructions)
- [W3C: Clear instructions](https://www.w3.org/WAI/WCAG2/supplemental/patterns/o4p07-step-instructions/)
- [GOV.UK: Details](https://design-system.service.gov.uk/components/details/)
- [W3C: Consistent Help](https://www.w3.org/WAI/WCAG22/Understanding/consistent-help/)

These principles are adapted to a physical-key handheld. They do not establish web-accessibility conformance or prove physical readability.

## Executable examples and language books

`docs/help/command-examples.json` supplies per-entry RPL programs, exact expected output and side-effect notes. Run `tools/test_help_examples.py`; its file-input mode avoids confusing equality signs with simulator CLI settings. A fresh stack/config is used for every example. `docs/help/lua-reference.json` and `chip8-reference.json` supply each language page, example and expected visible behavior. Generate with `tools/generate_language_references.py`. Run `tools/test_language_examples.py` for the actual production runtimes, noting its explicit hardware-fixture scope. Do not convert a failed example into an expected error without checking the command contract.


Language references use explicit sections in `docs/help/language-navigation.json`. Start with a short, runnable walkthrough, including the on-device route and expected result. Keep common tasks ahead of API lookup. Preserve canonical command names as search aliases while using task titles for guides. Label complete programs and instruction fragments distinctly; a fragment must not imply it can run alone. Describe the exact compatibility profile and save/run actions needed. Test examples with the production runtime and review the paged text on the native renderer.
