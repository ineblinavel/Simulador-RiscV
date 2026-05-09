# Melhorias recomendadas (estado atual)

Este documento lista somente os próximos pontos de melhoria, considerando o que já foi implementado.

## 1. Prioridade alta (consistência do estado da CPU)

1. **Remover estado global legado de `global.h/global.cpp`**
   - Hoje coexistem `cpu_state` e globais antigos (`Reg`, `Mem`, `pc`, `ri`, `Out`).
   - Isso abre espaço para divergência de estado.
   - Próximo passo: deixar apenas `cpu_state` como fonte única da verdade.

2. **Corrigir escrita de erro no `execute()`**
   - No `catch`, ainda existe `Out = 3;` em vez de `cpu_state.Out = OUT_ERROR;`.
   - Isso quebra a consistência da migração para `CpuState`.

3. **Encapsular também o contexto de instrução (`ic`) dentro da CPU**
   - `ic` ainda é global independente.
   - Ideal: mover para dentro de `CpuState` (ou de um módulo CPU) e remover dependência global.

## 2. Prioridade média (tipagem e API)

1. **Fortalecer tipagem no decode/execute**
   - `get_imm` recebe `UWord` para formato; pode receber `InstructionType`.
   - `execute()` usa `UWord instrucao = ic.ins_code;`; pode usar `Opcode` direto.

2. **Padronizar status com enum em todos os checks**
   - Em `cpu.cpp`, alguns trechos ainda usam `0` em vez de `RUNNING`.
   - Trocar por enum melhora legibilidade e evita erro semântico.

3. **Separar tipos/constantes comuns de estado global**
   - `Opcode`, `InstructionType`, limites de memória e aliases de tipo podem ir para um header de tipos (`types.h`/`isa.h`), evitando acoplamento com globais antigos.

## 3. Prioridade média (robustez funcional)

1. **Validar carregamento por segmento, não só por `MEM_SIZE`**
   - `loadmemory` valida limite total da memória, mas não impõe explicitamente fronteiras de `.text` e `.data`.
   - Melhor bloquear overflow entre segmentos.

2. **Falhar explicitamente ao não abrir binários**
   - Quando `code.bin`/`data.bin` falham, o estado de saída pode ficar ambíguo.
   - Definir `cpu_state.Out = OUT_ERROR` no carregamento.

3. **Política de alinhamento para `LW/SW`**
   - Definir se acessos desalinhados serão permitidos ou tratados como erro.
   - Hoje isso não está explicitado.

## 4. Testes e DX

1. **Criar `make test` com verificação automática**
   - Consolidar build + execução + comparação de saída esperada.

2. **Adicionar testes de regressão para a migração de `CpuState`**
   - Casos de erro (`OUT_ERROR`), opcode inválido, falha de carregamento e fim de memória.

3. **Adicionar modo trace opcional**
   - Exibir `pc`, opcode decodificado e registradores alterados por ciclo para facilitar depuração.

---

## Ordem recomendada de implementação

1. Fechar consistência (`cpu_state` único + correção do `catch`).
2. Mover `ic` para dentro da CPU e reduzir globais.
3. Reforçar tipagem (`Opcode`/`InstructionType`) e enum de status.
4. Melhorar carregamento por segmento e suíte de testes automatizada.
