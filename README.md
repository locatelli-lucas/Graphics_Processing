<h1>Graphics Processing</h1>
Este repositório contém implementações para atividades acadêmicas relacionadas ao processamento gráfico. Os exemplos a seguir demonstram diferentes conceitos utilizando OpenGL e GLFW.

<h2>Exercícios M2</h2>

<h3>ex1.cpp</h3>

Descrição: Este exemplo cria uma janela OpenGL que apresenta um único triângulo renderizado na tela. Ele demonstra o uso básico de shaders (vertex e fragment shaders) para aplicar uma cor ao triângulo e manipulá-lo usando transformações de projeção e modelo.
Principais Funcionalidades:
Criação de shaders básicos.
Renderização de um triângulo simples.
Configuração de projeções ortográficas.
Exemplo de manipulação de viewport.
Localização: src/M2/ex1/ex1.cpp

<h3>ex2.cpp</h3>

Descrição: Este exemplo expande o conceito do exemplo anterior para renderizar múltiplos triângulos na mesma janela. Ele utiliza um vetor de objetos VAO (Vertex Array Objects) para armazenar e renderizar vários triângulos, cada um com uma posição diferente.
Principais Funcionalidades:
Renderização de múltiplos triângulos com diferentes coordenadas.
Uso de arrays para gerenciar múltiplos objetos gráficos.
Configuração de shaders e projeções ortográficas.
Localização: src/M2/ex2/ex2.cpp

<h3>ex3.cpp</h3>

Descrição: Este exemplo introduz interatividade ao renderizador gráfico. Ele permite adicionar triângulos na tela com base no clique do mouse, além de alterar suas cores de maneira aleatória. Também inclui um callback para manipulação de teclas e eventos de mouse.
Principais Funcionalidades:
Adicionar triângulos dinamicamente na tela com base em eventos de clique.
Escolha aleatória de cores para os triângulos.
Uso de callbacks para capturar eventos do teclado e mouse.
Exemplo de manipulação de transformações (translação, rotação, escala) em triângulos individuais.
Localização: src/M2/ex3/ex3.cpp

<h3>Vivencial<h3>

Descrição: Este desafio contém uma lógica na qual, ao clicar três vezes na tela, um triângulo de cor aleatória é renderizdo na tela, e assim consecutivamente.

<h3>Jogo das Cores</h3>

Descrição: O usuário deve clicar em um quadrado da grade para escolher uma cor de referência. Todos os quadrados com cor semelhante à escolhida, dentro de um limite de similaridade pré-definido, são removidos da tela e somam pontos ao jogador. A cada tentativa, a pontuação máxima diminui. O jogo pode ser reiniciado a qualquer momento pressionando a tecla R. O objetivo é remover todos os quadrados com o maior número de pontos possível.
Principais Funcionalidades:
- Grade de quadrados coloridos gerados aleatoriamente.
- Seleção de cor por clique do mouse.
- Remoção automática de quadrados com cor semelhante à escolhida.
- Sistema de pontuação: pontos por acerto, penalidade por erro, decremento a cada tentativa.
- Reinício do jogo com a tecla R.
- Exibição da pontuação final ao término do jogo.
Localização: src/M3/JogoCores.cpp

<h3>ObjetosTexturizados.cpp</h3>

Descrição: Demonstra o uso de sprites e texturas em OpenGL, incluindo carregamento de imagens, aplicação de texturas em objetos, manipulação de múltiplos sprites e fundo, além de blending para transparência. O fundo é uma imagem que ocupa toda a tela, enquanto os sprites de personagens são desenhados alinhados e com tamanho padronizado. O exemplo também mostra movimentação de um sprite controlado pelo teclado.

Principais Funcionalidades:
- Carregamento de múltiplas texturas (fundo e sprites).
- Renderização de um fundo texturizado ocupando toda a janela.
- Renderização de vários sprites (personagens) alinhados e com o mesmo tamanho.
- Sprite do jogador controlado pelo teclado (movimento horizontal).
- Uso de blending para sprites com transparência.
- Exemplo de matriz de modelo para posicionamento e escala dos sprites.

Localização: src/M4/ObjetosTexturizados.cpp

<h3>gb.cpp</h3>

Descrição: Um jogo de tilemap isométrico onde o jogador deve coletar todas as moedas sem pisar na lava. O mapa é carregado de um arquivo texto, e cada tile pode conter moedas, água (intransponível), chão, ou lava (game over ao pisar). O personagem é animado e há animação para as moedas.

Principais Funcionalidades:
- Leitura de mapas customizados de arquivo.
- Tiles animados e sprites animados para o personagem.
- Sistema de coleta de moedas e detecção de vitória/derrota.
- Tiles de água bloqueiam o movimento.
- Lava não pode limitar todo o mapa, garantindo sempre caminhos possíveis.
- Reinício do jogo com a tecla R.

Como executar:
1. Compile o projeto usando CMake conforme instruções do repositório.
2. Execute o binário a partir da raiz do projeto:
   ```
   build\gb.cpp.exe
   ```
3. Certifique-se de que a pasta `assets` está na raiz, com todos os arquivos necessários.

Localização: src/GB/gb.cpp

Alunos: Lucas Locatelli, Bruno Gerling