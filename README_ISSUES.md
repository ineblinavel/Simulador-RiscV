# Issues sugeridas para o projeto

Este arquivo contém issues prontas para copiar e colar no GitHub.

---

## 1) Mover enums/tipos da ISA para `include/isa.h`

**Título:** `refactor: separar tipos da ISA de global.h`

**Descrição:**
Hoje `include/global.h` concentra aliases de tipo e enums da ISA (`Opcode`, `InstructionType`). Como o estado global já foi removido, o nome `global.h` ficou genérico e confuso.

Proposta:
- criar `include/isa.h` para concentrar:
  - aliases (`UByte`, `Word`, etc.),
  - constantes de memória (`MEM_SIZE`, limites),
  - enums da ISA.
- manter `global.h` mínimo (ou eliminar, se não houver mais uso).

**Critérios de aceitação:**
- [ ] `Opcode` e `InstructionType` não estão mais em `global.h`
- [ ] `cpu.h`, `decode.cpp` e `execute.cpp` compilam usando o novo header
- [ ] build e `make test` passam

---

## 2) Melhorar modelo de `ic_t` dentro de `CpuState`

**Título:** `refactor: substituir struct anônima ic_t por tipo nomeado`

**Descrição:**
`CpuState` usa uma `struct` anônima chamada `ic_t`. Isso funciona, mas dificulta reutilização e documentação.

Proposta:
- extrair para `struct InstructionContext` nomeada;
- usar `InstructionContext ic;` dentro de `CpuState`;
- atualizar `decode/execute`.

**Critérios de aceitação:**
- [ ] existe tipo nomeado para contexto de instrução
- [ ] `cpu_state.ic` substitui `cpu_state.ic_t`
- [ ] sem mudança de comportamento (testes continuam verdes)

---

## 3) Validar carga por segmento (.text/.data), não só por `MEM_SIZE`

**Título:** `fix: validar limites por segmento durante loadmemory`

**Descrição:**
`loadmemory` hoje valida apenas `state.pc + 3 < MEM_SIZE`. Isso não impede overflow entre segmentos lógicos `.text` e `.data`.

Proposta:
- validar `.text` até `INSTRUCTION_MEMORY_LIMIT`;
- validar `.data` no intervalo de dados;
- em erro, sinalizar `state.Out = OUT_ERROR`.

**Critérios de aceitação:**
- [ ] overflow de `.text` é detectado corretamente
- [ ] overflow de `.data` é detectado corretamente
- [ ] `Out` fica em `OUT_ERROR` nos casos inválidos

---

## 4) Falha explícita no carregamento de `code.bin`/`data.bin`

**Título:** `fix: definir OUT_ERROR quando arquivo de entrada não abrir`

**Descrição:**
Quando `code.bin` ou `data.bin` não abre, a função retorna após log, mas sem sempre garantir estado de erro padronizado.

Proposta:
- ao falhar abertura/leitura crítica, setar `state.Out = OUT_ERROR`;
- manter mensagem de erro clara.

**Critérios de aceitação:**
- [ ] erro de arquivo não deixa execução em estado ambíguo
- [ ] `run()` encerra com mensagem de falha consistente

---

## 5) Tipagem forte em decode/execute

**Título:** `refactor: usar enums diretamente em get_imm e execute`

**Descrição:**
Ainda há assinaturas com tipos amplos onde enums cabem melhor (`get_imm(..., UWord instr_type)` e `UWord instrucao` no execute).

Proposta:
- trocar para `InstructionType` em `get_imm`;
- usar `Opcode` diretamente em `execute`.

**Critérios de aceitação:**
- [ ] assinaturas usam enums corretos
- [ ] sem cast desnecessário
- [ ] build com `-Wall -Wextra` limpo

---

## 6) Testes negativos diferenciais (erro esperado)

**Título:** `test: adicionar casos diferenciais para cenários de erro`

**Descrição:**
A suíte atual cobre fluxo feliz bem, mas faltam casos de erro (opcode inválido, acesso fora da faixa de dados etc.).

Proposta:
- adicionar ASM/casos específicos para validar término com erro;
- adaptar runner para suportar testes com resultado esperado de falha.

**Critérios de aceitação:**
- [ ] existe pelo menos 1 caso de opcode inválido
- [ ] existe pelo menos 1 caso de acesso inválido de memória
- [ ] runner diferencia PASS por falha esperada vs falha inesperada

---

## 7) Publicar artefatos de diff no GitHub Actions

**Título:** `ci: anexar diffs de testes diferenciais como artifact`

**Descrição:**
Quando `make test` falha no CI, os diffs ficam no runner e não são fáceis de inspecionar no PR.

Proposta:
- no workflow, fazer upload de `build/test-artifacts` quando houver falha.

**Critérios de aceitação:**
- [ ] em falha de teste, artifacts são publicados no run
- [ ] PR permite baixar diff sem reproduzir localmente

---

## 8) Implementar Extensão M (Multiplicação e Divisão)

**Título:** `feat: implementar suporte à extensão RV32M`

**Descrição:**
Atualmente o simulador suporta apenas o set básico `RV32I`. Para rodar binários mais complexos, é necessário suporte a operações de hardware para multiplicação e divisão.

Proposta:
- Adicionar opcodes da extensão M (`MUL`, `DIV`, etc.) ao `include/isa.h`;
- Implementar a lógica de execução em `src/execute.cpp`;
- Adicionar casos de teste em `teste/cases/rv32m.asm`.

**Critérios de aceitação:**
- [ ] Opcodes da extensão M reconhecidos no `decode`
- [ ] Resultados de multiplicação/divisão validados contra RARS
- [ ] Divisão por zero tratada conforme especificação RISC-V

---

## 9) Monitoramento de Desempenho (Hardware Counters)

**Título:** `feat: adicionar contadores de desempenho (instret e cycles)`

**Descrição:**
Rastrear o número de instruções executadas e ciclos para análise de eficiência (simulando CSRs como `mcycle`).

Proposta:
- Adicionar `instret` e `cycles` à struct `CpuState`;
- Incrementar contadores a cada instrução;
- Exibir resumo estatístico ao final da execução.

**Critérios de aceitação:**
- [ ] Simulador imprime total de instruções ao encerrar
- [ ] Contador ignora instruções que resultaram em erro
- [ ] Exibição opcional de IPC (Instruções por Ciclo)

---

## 10) Logs Coloridos e Níveis de Verbose

**Título:** `enhancement: implementar sistema de logging com níveis de verbosidade`

**Descrição:**
Permitir filtrar a saída do simulador e usar cores ANSI para destacar erros e avisos.

Proposta:
- Criar sistema de log com níveis (`INFO`, `DEBUG`, `WARN`, `ERROR`);
- Usar cores ANSI no terminal;
- Adicionar flag `--verbose` para controle de saída.

**Critérios de aceitação:**
- [ ] Erros em vermelho, avisos em amarelo no terminal
- [ ] Saída padrão limpa quando verbose está desligado
- [ ] Código de log centralizado

---

## 11) Tratamento de Alinhamento de Memória (Alignment Check)

**Título:** `fix: validar alinhamento de memória em instruções Load/Store`

**Descrição:**
Instruções `LW` e `SW` exigem endereços múltiplos de 4. Acessos desalinhados devem gerar erro no simulador.

Proposta:
- Verificar alinhamento (`address % 4 == 0`) em `execute.cpp`;
- Caso desalinhado, setar `state.Out = OUT_ERROR` e reportar "Alignment Fault".

**Critérios de aceitação:**
- [ ] `LW` em endereço ímpar gera erro imediato
- [ ] Mensagem de erro indica o endereço e a instrução ofensora
- [ ] Teste diferencial valida comportamento de falha

---

## 12) Desmontagem em Tempo Real (Tracer)

**Título:** `feat: implementar modo trace com desmontagem de instruções`

**Descrição:**
Exibir a instrução em formato assembly legível durante a execução (Tracer/Disassembler).

Proposta:
- Criar função para converter `InstructionContext` em string assembly;
- Implementar flag `--trace` para logar cada passo da CPU.

**Critérios de aceitação:**
- [ ] Saída formatada: `[PC] BINÁRIO | ASSEMBLY`
- [ ] Suporte a todo o set RV32I implementado
- [ ] Performance não afetada com trace desligado