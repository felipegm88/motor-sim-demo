# Coverage

The repository enforces **100% line coverage** for its own `src/` (excluding `src/main.c`).

## Generate coverage + HTML

```bash
west twister -T tests -p native_sim -v \
  --outdir twister-out-coverage \
  --coverage --coverage-tool gcovr --coverage-formats html
```

HTML report:

- `twister-out-coverage/coverage/index.html`

## Terminal summary (repo code only)

```bash
gcovr twister-out-coverage \
  --root . \
  --filter '^src/' \
  --exclude '^src/main\\.c$' \
  --exclude '^tests/' \
  --print-summary \
  --fail-under-line 100
```

## Branch coverage vs line coverage

- **Line coverage** answers: “Did we execute this line at least once?”
- **Branch coverage** answers: “Did we execute all outcomes of each decision?” (true/false branches, switch cases, short-circuit paths, etc.)

Branch coverage is usually harder to push to 100% in embedded-style code without explicit fault-injection for rare error paths.
