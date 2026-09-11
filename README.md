# Pathfinder3D

Visualizador 3D de algoritmos de busca de caminho (pathfinding) em uma grid, feito em C++17 com OpenGL/GLUT. Permite montar paredes, definir início e destino, rodar diferentes algoritmos e acompanhar a exploração acontecendo célula por célula em tempo real.

## Origem

Projeto idealizado para a disciplina de Computação Gráfica da UFAPE, desenvolvido por:

| Nome | GitHub |
|---|---|
| Arthur Passos | [@ArthurRLZ](https://github.com/ArthurRLZ) |
| Euclides Laurindo | [@euclideslaurindo](https://github.com/euclideslaurindo) |
| Heitor Calado | [@heitorcalado](https://github.com/heitorcalado) |
| Luis Arthur | [@lu1s-4rthur](https://github.com/lu1s-4rthur) |

## Funcionalidades atuais

- Grid 2D navegável em 3D, com paredes, ponto de partida (`Start`) e destino (`Goal`) editáveis com o mouse.
- Algoritmos de busca implementados: **BFS**, **A\***, **DFS** e **Dijkstra**.
- **Animação da busca em tempo real**: as células exploradas acendem em laranja conforme o algoritmo avança, e o caminho final encontrado é destacado em ciano por cima da exploração.
- Métricas impressas no console ao final de cada busca: número de passos do caminho, quantidade de células visitadas e tempo de computação.
- Câmera orbital: sempre mira o centro da grid, girando livremente ao redor dele (sem zoom nem translação livre).
- Botão para alternar entre a órbita livre (3D) e uma vista de topo travada (2D), no overlay ao lado do minimapa, a transição é animada, não um corte seco.
- Menu inicial para escolher o tamanho do grid (NxN, mínimo 10x10) e marcar quais algoritmos entram na comparação.
- Modo de comparação: roda todos os algoritmos marcados sobre a mesma grid/obstáculos/Start/Goal e mostra uma tabela com se achou caminho, tamanho do caminho, células visitadas, tempo de computação e uma miniatura do padrão de busca de cada um (igual o minimapa mostraria célula por célula).
- Geração automática de paredes (opcional, via menu): cria cavernas orgânicas com autômato celular, sempre garantindo que exista caminho entre Start e Goal.
- Minimapa 2D no canto da tela com legenda dos controles.

## Requisitos

- Compilador C++17 (GCC, Clang ou MSVC).
- CMake ≥ 3.10.
- OpenGL e GLUT.
  - **Linux (Debian/Ubuntu):** `sudo apt install freeglut3-dev`
  - **Windows:** instale o freeglut (via [vcpkg](https://vcpkg.io) ou MSYS2) e garanta que `OpenGL`/`GLUT` sejam encontrados pelo `find_package` do CMake.
  - **macOS:** o framework OpenGL já vem no sistema; instale o GLUT via Homebrew (`brew install freeglut`).

## Como compilar

```bash
cmake -B build
cmake --build build
```

O executável `pathfinder3d` é gerado dentro da pasta `build/`.

> **Atenção:** não versione a pasta `build/` (ou `cmake-build-debug/`, se estiver usando CLion). O `CMakeCache.txt` guarda o caminho absoluto de onde foi gerado, então uma pasta de build criada em uma máquina/usuário não funciona em outra, se isso acontecer, apague a pasta e rode `cmake -B build` de novo.

## Como rodar

```bash
./build/pathfinder3d
```

A janela abre em **1920x1080** por padrão (definido em `Renderer::run()`, via `glutInitWindowSize`). Para mudar, edite os dois números em `glutInitWindowSize(1920, 1080)` em `src/view/Renderer.cpp` e os valores iniciais de `windowWidth`/`windowHeight` em `src/view/Renderer.h` (mantém os dois em sincronia, já que são usados antes do primeiro redimensionamento da janela). O layout do menu e dos resultados já é recalculado a cada frame a partir do tamanho atual da janela, então funciona em qualquer resolução — só o tamanho em pixels dos cards/botões continua fixo, então em telas muito grandes eles aparecem proporcionalmente menores.

O programa abre direto na **tela de menu**: escolha o tamanho do grid (botões `-`/`+`) e marque quais algoritmos vão entrar nas comparações, depois clique em "Confirmar" (ou aperte `Enter`) para ir para a simulação. Esses algoritmos marcados são justamente os que rodam quando você aperta `C` mais tarde, na simulação, mudar essa seleção exige voltar ao menu.

## Controles

### Tela de menu

| Ação | Efeito |
|---|---|
| Clique em `-` / `+` | Diminui/aumenta o tamanho do grid (NxN, de 10x10 até 60x60) |
| Clique num algoritmo | Marca/desmarca ele para entrar no modo de comparação |
| Clique em "Gerar paredes automaticamente" | Liga/desliga a geração automática de labirinto para essa grid |
| Clique em "Confirmar" / `Enter` | Cria o grid no tamanho escolhido (gerando paredes automaticamente, se marcado) e vai para a simulação |

### Tela de simulação

| Tecla / Ação | Efeito |
|---|---|
| Clique na grid (modo Start) | Define a célula de início |
| Clique na grid (modo Goal) | Define a célula de destino |
| Clique na grid (modo Wall) | Alterna parede na célula |
| Clique na grid (modo Erase) | Limpa a célula |
| `S` | Entra no modo "definir Start" |
| `G` | Entra no modo "definir Goal" |
| `W` | Entra no modo "desenhar parede" |
| `E` | Entra no modo "apagar/borracha" |
| `1` | Seleciona o algoritmo BFS (para rodar com Enter) |
| `2` | Seleciona o algoritmo A\* (para rodar com Enter) |
| `3` | Seleciona o algoritmo DFS (para rodar com Enter) |
| `4` | Seleciona o algoritmo Dijkstra (para rodar com Enter) |
| `Enter` | Roda o algoritmo selecionado, animado (Start e Goal precisam estar definidos) |
| `C` | Roda o modo comparação: todos os algoritmos marcados no menu, sem animação, com tabela de resultados |
| `R` | Reseta a grid (cancela uma busca em andamento, se houver) |
| Setas ← → / `A` / `D` | Gira a câmera em torno da grid |
| Setas ↑ ↓ | Inclina a câmera (eleva/abaixa o ângulo de visão) |
| Clique no minimapa | Equivalente a clicar na célula correspondente da grid |
| Clique no botão "Vista 2D/3D" (abaixo da legenda) | Alterna entre a órbita livre e a vista de topo travada |
| Clique no botão "Voltar ao menu" (abaixo do de vista 2D/3D) | Volta para a tela de menu, mantendo a grid atual até você confirmar um novo tamanho |

Durante uma busca em andamento (`Enter`), edições na grid (paredes, Start, Goal) ficam bloqueadas até ela terminar ou ser cancelada com `R`. O modo comparação (`C`) roda tudo de uma vez, sem animação, é só para medir.

### Tela de resultados

| Ação | Efeito |
|---|---|
| Clique em "Voltar" / qualquer tecla | Volta para a simulação, mantendo a grid como estava |
| Clique em "Voltar ao menu" | Volta para a tela de menu, mantendo a grid atual até você confirmar um novo tamanho |

## Paleta de cores

A interface (menu, tela de resultados, cards, botões, switches) usa uma paleta escura baseada em slate & indigo, com cantos arredondados, sombras suaves e um leve "glow" no botão em destaque, tudo desenhado à mão em OpenGL modo imediato (GLUT não tem nenhum toolkit de widgets).

**As cores da própria grid (parede, início, fim, visitado, caminho) ficaram de fora desse redesign, de propósito**: usam matizes bem distintos entre si (vermelho/verde/azul/ciano/laranja) em vez de tons de uma mesma família de cor, porque aqui a cor carrega informação funcional, não é só estética, precisa dar pra reconhecer cada categoria de relance, inclusive nas miniaturas pequenas da tela de comparação. Uma versão anterior chegou a aplicar a paleta indigo também na grid, mas ficou difícil distinguir parede/visitado/vazio a olho nu nas miniaturas (todos viravam tons parecidos de cinza-azulado); por isso a decisão de manter essa parte separada.

## Estrutura do projeto

Arquitetura em MVC:

```
src/
├── app/
│   └── main.cpp                 # Ponto de entrada: cria Grid, Controller, Renderer e Camera
├── model/
│   ├── Grid/
│   │   └── Grid.h/.cpp           # Matriz de células e operações básicas de acesso
│   ├── Cell/
│   │   ├── Cell.h                # Struct simples (x, y) usada como coordenada
│   │   └── CellType.h            # Enum dos tipos de célula (Empty, Wall, Start, Goal, Visited, Path)
│   └── Algorithms/
│       ├── ISearchAlgorithm.h    # Interface comum para algoritmos de busca "stepáveis"
│       ├── AlgorithmType.h       # Enum dos algoritmos disponíveis (BFS, ASTAR, DFS, DIJKSTRA)
│       ├── AlgorithmFactory/
│       │   └── AlgorithmFactory.h/.cpp # Cria a instância concreta a partir do AlgorithmType
│       ├── BFS/
│       │   └── BFS.h/.cpp        # Implementação de BFS sobre a interface
│       ├── AStar/
│       │   └── AStar.h/.cpp      # Implementação de A* sobre a interface
│       ├── DFS/
│       │   └── DFS.h/.cpp        # Implementação de DFS sobre a interface (mesma estrutura do BFS, com pilha em vez de fila)
│       └── Dijkstra/
│           └── Dijkstra.h/.cpp   # Implementação de Dijkstra sobre a interface (A* com heurística 0)
│   ├── WallGenerator.h/.cpp      # Geração automática de paredes (autômato celular) + validação de conectividade
├── controller/
│   └── Controller.h/.cpp         # Trata input (teclado/mouse), edição da grid, orquestra a execução do algoritmo e o estado da aplicação (Menu/Simulation/Results)
└── view/
    ├── Camera.h/.cpp             # Câmera orbital (gira em torno do centro fixo da grid)
    └── Renderer.h/.cpp           # Loop do GLUT, desenho da cena 3D, do minimapa, do menu e da tela de resultados
```

Cada algoritmo mora na sua própria pasta dentro de `model/Algorithms/`, junto com a fábrica (`AlgorithmFactory/`) que decide qual instanciar a partir do `AlgorithmType`. Isso deixa claro, só pela árvore de pastas, que adicionar um algoritmo novo é: criar uma pasta nova aqui dentro, implementar `ISearchAlgorithm`, e adicionar um `case` na fábrica, sem tocar em `Controller` além do `enum` e da tecla correspondente.

### Como funciona a animação da busca

Os algoritmos não rodam do início ao fim de uma vez: cada um implementa `ISearchAlgorithm`, com um método `step()` que processa uma unidade de trabalho por chamada (por exemplo, um nó da fronteira de busca). O `Controller` usa `glutTimerFunc` para chamar `step()` periodicamente e pedir um redesenho (`glutPostRedisplay`) a cada chamada, o que produz a animação célula por célula. Quando `step()` sinaliza que terminou, o `Controller` pinta o caminho encontrado (se houver) e imprime as métricas coletadas (`SearchStats`: células visitadas e tempo de computação).

Essa interface foi pensada para facilitar a adição de novos algoritmos sem precisar reescrever a lógica de animação ou de coleta de métricas, só implementar `start()`/`step()` seguindo o mesmo contrato. Adicionar um algoritmo novo à interface (`Controller`, teclado, minimapa) é uma questão de: implementar a classe, adicionar um valor ao enum `AlgorithmType` e um `case` em `AlgorithmFactory::createAlgorithm`.

> **Nota sobre o DFS:** diferente de BFS/A*/Dijkstra, o DFS não garante o caminho mais curto — ele só garante *algum* caminho, se existir. É esperado que o caminho encontrado pelo DFS seja mais longo (às vezes bem mais longo) que o dos outros três ao comparar resultados.

### Como funciona o menu e o modo de comparação

O `Controller` guarda um `AppState` (`Menu`, `Simulation` ou `Results`) que decide o que o `Renderer` desenha e como ele interpreta cliques do mouse, a tela de menu e a de resultados são overlays 2D em tela cheia (mesmo esquema `gluOrtho2D` já usado no minimapa), não janelas ou widgets nativos.

No menu, escolher "Confirmar" chama `Grid::resize()`, que realoca a matriz interna da própria instância de `Grid`, como o `Controller` guarda uma referência (`Grid&`), não é preciso recriar nada. A câmera também é recentralizada nesse momento (`Camera::setOrbit`), já que o centro geométrico do grid muda de tamanho junto.

O modo de comparação (`C`, dentro da simulação) roda cada algoritmo marcado no menu **sem animação**: cria uma cópia independente da `Grid` atual (mesmas paredes/Start/Goal), instancia o algoritmo via `createAlgorithm()` e chama `step()` num laço fechado até terminar, em vez de usar `glutTimerFunc` como a busca animada faz. Isso mede o tempo de computação de verdade, sem o atraso artificial entre frames da animação.

Antes de rodar cada algoritmo, a cópia da grid é higienizada: qualquer célula `Visited`/`Path` que tenha sobrado de uma execução interativa anterior (`Enter`) na grid real é revertida para `Empty`. Sem isso, BFS e DFS, que só andam por cima de `CellType::Empty` ou `CellType::Goal`, tratariam essas células como bloqueio (igual parede), dando resultado incorreto e inconsistente com A*/Dijkstra (que só recusam `CellType::Wall`, então atravessam esses restos sem problema). `startAlgorithm()` já faz essa mesma limpeza antes de cada corrida interativa; o modo de comparação replica a mesma lógica na cópia.

Depois que cada algoritmo termina, o caminho encontrado é pintado por cima da cópia (`CellType::Path`), e essa cópia inteira, paredes, células visitadas e caminho, fica guardada em `ComparisonRow::snapshot`. É esse "retrato" que a tela de Resultados desenha como uma miniatura por linha da tabela (`Renderer::drawGridThumbnail`), usando exatamente as mesmas cores do minimapa da simulação (`CellType::Wall` vermelho escuro, `Visited` laranja, `Path` ciano, `Start`/`Goal` verde/azul). Isso deixa visualmente óbvio o padrão de busca de cada algoritmo lado a lado, a "onda" do BFS/Dijkstra, o corredor fino e sinuoso do DFS, a pegada bem mais enxuta do A*.

### Como funciona a geração automática de paredes

`gerarParedes()` usa autômato celular (estilo "cave generation"): preenche cada célula da grid como parede com ~42% de probabilidade, depois roda 5 passadas de suavização onde cada célula vira parede se tiver 5 ou mais vizinhas-parede (vizinhança 8-direcional), isso transforma o ruído aleatório inicial em formas orgânicas de caverna, em vez de um campo minado sem padrão.

O autômato celular sozinho tende a empurrar a "massa" de parede pra perto das bordas (a regra "fora da grid conta como parede" dá um empurrão extra às células perto da borda a cada passada) e deixar o interior praticamente vazio, principalmente em grids pequenas/médias, onde "perto da borda" já é uma fração grande da área total. Pra compensar isso, depois da suavização a função espalha alguns blocos retangulares de parede pelo interior (`sprinkleInteriorBlobs`), garantindo obstáculos de verdade no meio do caminho em vez de só uma moldura oca. Grids menores que 8x8 pulam essa etapa (não sobra espaço de sobra pra blocos com margem).

Como essa função roda logo após `grid.resize()` no menu, **ainda não existe Start/Goal do usuário** para reaproveitar, a própria função escolhe cantos opostos (ou a célula vazia mais próxima de cada canto, caso ele tenha virado parede) e valida a conectividade entre eles usando o `BFS` já implementado, rodado até o fim sem timer, do mesmo jeito que o modo de comparação faz. Se não houver caminho, tenta gerar tudo de novo (até 20 vezes); se mesmo assim falhar, cai para uma grid vazia em vez de deixar o usuário preso numa configuração sem solução. Isso só seria um problema em grids muito pequenas (2x2/3x3), mas o menu já não permite ir abaixo de 10x10, nesse tamanho pra cima, a geração praticamente sempre conecta de primeira.

Um detalhe sutil que vale registrar: `BFS::step()` reconhece que chegou ao destino comparando o **tipo da célula** (precisa ser `CellType::Goal`), não as coordenadas. Por isso, tanto `gerarParedes()` quanto o modo de comparação precisam marcar a célula de destino como `CellType::Goal` *antes* de rodar a busca de validação, sem isso, o BFS explora a grid inteira sem nunca detectar que chegou lá.