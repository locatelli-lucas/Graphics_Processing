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
