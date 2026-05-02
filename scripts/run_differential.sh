#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RARS_JAR="${RARS_JAR:-$ROOT_DIR/rars1_6.jar}"
ARTIFACTS_DIR="$ROOT_DIR/build/test-artifacts"
CODE_BIN="$ROOT_DIR/teste/code.bin"
DATA_BIN="$ROOT_DIR/teste/data.bin"

if [[ ! -f "$RARS_JAR" ]]; then
  echo "Erro: RARS não encontrado em '$RARS_JAR'. Defina RARS_JAR=/caminho/rars.jar" >&2
  exit 2
fi

cd "$ROOT_DIR"
mkdir -p "$ARTIFACTS_DIR"

mapfile -t ASM_FILES < <(find "$ROOT_DIR/teste/cases" -maxdepth 1 -type f -name "*.asm" 2>/dev/null | sort || true)
if [[ ${#ASM_FILES[@]} -eq 0 ]]; then
  ASM_FILES=("$ROOT_DIR/teste/teste.asm")
fi

echo "Executando ${#ASM_FILES[@]} teste(s) diferencial(is)..."

failures=0
for asm in "${ASM_FILES[@]}"; do
  name="$(basename "$asm" .asm)"
  ref_out="$ARTIFACTS_DIR/$name.ref.out"
  ref_err="$ARTIFACTS_DIR/$name.ref.err"
  sim_raw="$ARTIFACTS_DIR/$name.sim.raw.out"
  sim_out="$ARTIFACTS_DIR/$name.sim.out"
  ref_clean="$ARTIFACTS_DIR/$name.ref.clean.out"
  diff_out="$ARTIFACTS_DIR/$name.diff"

  rm -f "$CODE_BIN" "$DATA_BIN"
  java -jar "$RARS_JAR" a ae1 nc mc CompactTextAtZero dump .text Binary "$CODE_BIN" "$asm" >/dev/null 2>"$ref_err"
  java -jar "$RARS_JAR" a ae1 nc mc CompactTextAtZero dump .data Binary "$DATA_BIN" "$asm" >/dev/null 2>>"$ref_err"
  java -jar "$RARS_JAR" ae1 se1 nc me "$asm" >"$ref_out" 2>>"$ref_err"

  ./trabalho >"$sim_raw"

  # Remove linhas de encerramento/ruído para comparação estável.
  grep -v -E '^(-- program is finished running \(0\) --|Program terminated by calling exit|[[:space:]]*)$' "$ref_out" >"$ref_clean" || true
  grep -v -E '^(-- program is finished running \(0\) --|Program terminated by calling exit|[[:space:]]*)$' "$sim_raw" >"$sim_out" || true

  if diff -u "$ref_clean" "$sim_out" >"$diff_out"; then
    rm -f "$diff_out"
    echo "PASS $name"
  else
    echo "FAIL $name (diff: $diff_out)"
    failures=$((failures + 1))
  fi
done

if [[ $failures -ne 0 ]]; then
  echo "Falharam $failures teste(s)." >&2
  exit 1
fi

echo "Todos os testes diferenciais passaram."
