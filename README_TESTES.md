# README de Testes (Estratégia Diferencial)

Este documento descreve como testar o simulador comparando a saída dele com uma referência externa a partir do **mesmo `.asm`**.

## Objetivo

Para cada caso de teste:
1. Montar/gerar `code.bin` e `data.bin` a partir de um arquivo ASM.
2. Executar o ASM em uma referência (oracle).
3. Executar os mesmos binários no seu simulador.
4. Comparar as saídas.

Se as saídas diferirem, há regressão/bug.

---

## Abordagem recomendada para este projeto

Como seu simulador implementa ECALLs no estilo educacional (print int/string, exit), a referência mais simples é o **RARS**.

### Fluxo por teste

1. Arquivo de entrada: `teste/cases/<nome>.asm`
2. Referência:
   - Rodar o ASM no RARS e salvar saída em `build/<nome>.ref.out`
3. Simulador:
   - Gerar `teste/code.bin` e `teste/data.bin` a partir do mesmo ASM
   - Rodar `./trabalho` e salvar em `build/<nome>.sim.out`
4. Comparação:
   - `diff -u build/<nome>.ref.out build/<nome>.sim.out`

---

## Estrutura sugerida

```text
teste/
  cases/
    smoke.asm
    branches.asm
    memory.asm
  expected/              # opcional, snapshots de referência
scripts/
  run_differential.sh
build/
  test-artifacts/        # outputs gerados na execução
```

---

## Script implementado (sem framework)

O projeto já inclui `scripts/run_differential.sh` que:

1. Faça build do simulador (`make`).
2. Itere em `teste/cases/*.asm`.
3. Para cada caso:
   - rode referência (RARS) e salve stdout;
   - gere `code.bin` e `data.bin`;
   - rode `./trabalho`;
   - normalize saída (trim de espaços finais, quebra de linha final, etc.);
   - compare com `diff -u`.
4. Retorne código `0` se tudo passar, `1` se qualquer caso falhar.

---

## Como rodar

```bash
make test
```

Por padrão o script usa `./rars1_6.jar`.  
Para usar outro jar:

```bash
RARS_JAR=/caminho/para/rars.jar make test
```

---

## Opção com framework (quando quiser evoluir)

Você pode manter o comparador em shell/Python e usar um framework só para relatório:

- **Pytest** (mais simples para orquestrar processos e comparar arquivos).
- **GoogleTest/Catch2** (útil se quiser também testar funções C++ internas).

Recomendação prática: começar com **script + diff**, depois migrar para **pytest** quando a suíte crescer.

---

## Boas práticas para evitar falso negativo

1. Determinismo: evite testes dependentes de tempo/endereço variável.
2. Normalização: remova ruído de formatação antes do diff.
3. Casos pequenos e focados: um comportamento por teste.
4. Cobertura progressiva:
   - aritmética/lógica,
   - branch/jump,
   - memória (LB/LBU/LW/SB/SW),
   - ECALL,
   - erros (opcode inválido, faixa de memória).

---

## Exemplo de pipeline por caso

Para `teste/cases/memory.asm`:

1. `RARS -> memory.ref.out`
2. `RARS dump -> teste/code.bin + teste/data.bin`
3. `./trabalho -> memory.sim.out`
4. `diff -u memory.ref.out memory.sim.out`

Se o `diff` for vazio, o caso passou.

---

## Roadmap curto de implementação

1. Criar `teste/cases/` e mover casos atuais para lá.
2. Criar `scripts/run_differential.sh`.
3. Adicionar alvo `make test`.
4. Rodar no CI (GitHub Actions) com instalação do oracle escolhido.
