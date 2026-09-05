# Pathfinder3D

Visualizador 3D de algoritmos de busca de caminho (pathfinding) em uma grid, feito em C++17 com OpenGL/GLUT. Permite montar paredes, definir início e destino, rodar diferentes algoritmos e acompanhar a exploração acontecendo célula por célula em tempo real.

## Funcionalidades atuais

- Grid 2D navegável em 3D, com paredes, ponto de partida (`Start`) e destino (`Goal`) editáveis com o mouse.
- Algoritmos de busca implementados: **BFS**, **A\***, **DFS** e **Dijkstra**.
- **Animação da busca em tempo real**: as células exploradas acendem em laranja conforme o algoritmo avança, e o caminho final encontrado é destacado em ciano por cima da exploração.
- Métricas impressas no console ao final de cada busca: número de passos do caminho, quantidade de células visitadas e tempo de computação.
- Câmera livre (estilo FPS) com movimento e rotação.
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

> **Atenção:** não versione a pasta `build/` (ou `cmake-build-debug/`, se estiver usando CLion). O `CMakeCache.txt` guarda o caminho absoluto de onde foi gerado, então uma pasta de build criada em uma máquina/usuário não funciona em outra — se isso acontecer, apague a pasta e rode `cmake -B build` de novo.

## Como rodar

```bash
./build/pathfinder3d
```

## Controles

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
| `1` | Seleciona o algoritmo BFS |
| `2` | Seleciona o algoritmo A\* |
| `3` | Seleciona o algoritmo DFS |
| `4` | Seleciona o algoritmo Dijkstra |
| `Enter` | Roda o algoritmo selecionado (Start e Goal precisam estar definidos) |
| `R` | Reseta a grid (cancela uma busca em andamento, se houver) |
| Setas ↑ ↓ ← → | Move a câmera |
| `A` / `D` | Gira a câmera |
| Clique no minimapa | Equivalente a clicar na célula correspondente da grid |

Durante uma busca em andamento, edições na grid (paredes, Start, Goal) ficam bloqueadas até ela terminar ou ser cancelada com `R`.

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
├── controller/
│   └── Controller.h/.cpp         # Trata input (teclado/mouse), edição da grid e orquestra a execução do algoritmo
└── view/
    ├── Camera.h/.cpp             # Câmera livre (posição + yaw/pitch)
    └── Renderer.h/.cpp           # Loop do GLUT, desenho da cena 3D e do minimapa 2D
```

Cada algoritmo mora na sua própria pasta dentro de `model/Algorithms/`, junto com a fábrica (`AlgorithmFactory/`) que decide qual instanciar a partir do `AlgorithmType`. Isso deixa claro, só pela árvore de pastas, que adicionar um algoritmo novo é: criar uma pasta nova aqui dentro, implementar `ISearchAlgorithm`, e adicionar um `case` na fábrica — sem tocar em `Controller` além do `enum` e da tecla correspondente.

### Como funciona a animação da busca

Os algoritmos não rodam do início ao fim de uma vez: cada um implementa `ISearchAlgorithm`, com um método `step()` que processa uma unidade de trabalho por chamada (por exemplo, um nó da fronteira de busca). O `Controller` usa `glutTimerFunc` para chamar `step()` periodicamente e pedir um redesenho (`glutPostRedisplay`) a cada chamada, o que produz a animação célula por célula. Quando `step()` sinaliza que terminou, o `Controller` pinta o caminho encontrado (se houver) e imprime as métricas coletadas (`SearchStats`: células visitadas e tempo de computação).

Essa interface foi pensada para facilitar a adição de novos algoritmos sem precisar reescrever a lógica de animação ou de coleta de métricas — só implementar `start()`/`step()` seguindo o mesmo contrato. Adicionar um algoritmo novo à interface (`Controller`, teclado, minimapa) é uma questão de: implementar a classe, adicionar um valor ao enum `AlgorithmType` e um `case` em `AlgorithmFactory::createAlgorithm`.

> **Nota sobre o DFS:** diferente de BFS/A*/Dijkstra, o DFS não garante o caminho mais curto — ele só garante *algum* caminho, se existir. É esperado que o caminho encontrado pelo DFS seja mais longo (às vezes bem mais longo) que o dos outros três ao comparar resultados.

## Roadmap

- [x] Animar a exploração do algoritmo célula por célula em vez de só mostrar o resultado final.
- [x] Adicionar mais algoritmos de busca (DFS, Dijkstra).
- [ ] Trocar a câmera livre por uma câmera orbital, girando sempre em torno do centro da grid.
- [ ] Botão para alternar para uma vista 2D top-down.
- [ ] Menu inicial para escolher o tamanho da grid (NxN, mínimo 2x2) e selecionar quais algoritmos comparar (tempo, células visitadas, tamanho do caminho).
- [ ] Geração automática de paredes.

## Licença

Sem licença definida ainda.