// --- INCLUDE AND LIBRARY DEFINITIONS ---
#include <iostream> // Entrada e saída padrão
#include <vector>   // Vetores dinâmicos
#include <cmath>    // Funções matemáticas
#include <string>   // Manipulação de strings
#include <glad/glad.h> // OpenGL loader
#include <GLFW/glfw3.h> // Janela e contexto OpenGL
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // Carregamento de imagens
#include <glm/glm.hpp> // Biblioteca de álgebra linear
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>   // Leitura de arquivos
#include <sstream>   // Manipulação de strings
#include <algorithm> // Funções utilitárias
#include <chrono>    // Medição de tempo

// --- ESTRUTURA DE TILE ---
struct TileInfo { int tileIndex; bool hasCoin; };

// --- CONSTANTES DE TELA E TILE ---
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int BLOCK_WIDTH = 64;
const int BLOCK_HEIGHT = 32;

// --- VARIÁVEIS DE ESTADO DO JOGO ---
int heroCol = 1, heroRow = 1; // Posição do jogador
int worldRows = 0, worldCols = 0; // Tamanho do mapa
int coinsCollected = 0, coinsTotal = 0; // Contador de moedas
double gameStartTime = 0;
int coinAnimFrame = 0, idleAnimFrame = 0;
double coinAnimElapsed = 0, idleAnimElapsed = 0;
const double coinAnimInterval = 0.06, idleAnimInterval = 0.18;

std::string atlasFile;
std::vector<std::vector<TileInfo>> worldMap;
GLuint shaderCore, atlasTexture, tileVAO, tileVBO;
GLuint heroTexture, idleTexture;
GLuint coinSprites[10];
enum GameState { RUNNING, WON, GAMEOVER };
GameState playState = RUNNING;
glm::mat4 orthoProj;

GLuint createShader() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 model;
        uniform mat4 projection;
        out vec2 TexCoord;
        void main() {
            gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D tileset;
        uniform vec4 colorMod;
        void main() { FragColor = texture(tileset, TexCoord) * colorMod; }
    )";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}

// --- FUNÇÃO DE CARREGAMENTO DO MAPA ---
void loadMap(const std::string& path) {
    // Lê o arquivo de mapa e preenche a matriz de tiles
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Erro ao abrir map " << path << std::endl;
        return;
    }
    std::string line;
    std::getline(file, atlasFile);
    std::getline(file, line);
    std::istringstream sizeStream(line);
    sizeStream >> worldRows >> worldCols;
    worldMap.clear();
    for (int i = 0; i < worldRows; ++i) {
        std::getline(file, line);
        std::istringstream rowStream(line);
        std::vector<TileInfo> row;
        for (int j = 0; j < worldCols; ++j) {
            std::string token;
            rowStream >> token;
            TileInfo info;
            if (!token.empty() && token[0] == '/') continue; 
            if (!token.empty() && token.back() == 'c') {
                info.hasCoin = true;
                token.pop_back();
            } else {
                info.hasCoin = false;
            }
            if (!token.empty()) info.tileIndex = std::stoi(token);
            else                info.tileIndex = 0;
            row.push_back(info);
        }
        worldMap.push_back(row);
    }
}

// --- FUNÇÃO DE CARREGAMENTO DA TEXTURA DO TILES ---
void loadTilesetTexture(const std::string& path) {
    // Carrega a imagem do tileset e cria a textura OpenGL
    glGenTextures(1, &atlasTexture);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);    
    int imgW, imgH, imgChannels;
    unsigned char* pixels = stbi_load(path.c_str(), &imgW, &imgH, &imgChannels, 4);
    if (pixels) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    } else {
        std::cerr << "Erro ao carregar textura\n";
    }
}

// --- FUNÇÃO DE CARREGAMENTO DA TEXTURA DO JOGADOR ---
void loadPlayerTexture(const std::string& path) {
    // Carrega a imagem do personagem e aplica pré-processamento de alpha
    glGenTextures(1, &heroTexture);
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int imgW, imgH, imgChannels;
    unsigned char* pixels = stbi_load(path.c_str(), &imgW, &imgH, &imgChannels, 4);
    if (pixels) {
        for (int y = 0; y < imgH; ++y) {
            for (int x = 0; x < imgW; ++x) {
                int idx = (y * imgW + x) * 4;
                if (pixels[idx + 3] < 128) {
                    pixels[idx + 0] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 2] = 0;
                    pixels[idx + 3] = 0;
                } else {
                    float alpha = pixels[idx + 3] / 255.0f;
                    pixels[idx + 0] = (unsigned char)(pixels[idx + 0] * alpha);
                    pixels[idx + 1] = (unsigned char)(pixels[idx + 1] * alpha);
                    pixels[idx + 2] = (unsigned char)(pixels[idx + 2] * alpha);
                }
            }
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    } else {
        std::cerr << "Erro ao carregar textura do personagem\n";
    }
}

// --- FUNÇÃO DE CARREGAMENTO DAS TEXTURAS DAS MOEDAS ---
void loadCoinTextures() {
    // Carrega as imagens de animação das moedas
    char buf[128];
    for (int i = 0; i < 10; ++i) {
        sprintf(buf, "./assets/sprites/Bronze_%d.png", i+21);
        glGenTextures(1, &coinSprites[i]);
        glBindTexture(GL_TEXTURE_2D, coinSprites[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        int imgW, imgH, imgChannels;
        unsigned char* pixels = stbi_load(buf, &imgW, &imgH, &imgChannels, 4);
        if (pixels) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            stbi_image_free(pixels);
        } else {
            printf("Erro ao carregar %s\n", buf);
        }
    }
}

// --- FUNÇÃO DE CARREGAMENTO DA TEXTURA DE IDLE DO JOGADOR ---
void loadPlayerIdleTexture() {
    // Carrega a animação de idle do personagem
    glGenTextures(1, &idleTexture);
    glBindTexture(GL_TEXTURE_2D, idleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int imgW, imgH, imgChannels;
    unsigned char* pixels = stbi_load("./assets/sprites/Idle.png", &imgW, &imgH, &imgChannels, 4);
    if (pixels) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgW, imgH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    } else {
        printf("Erro ao carregar Idle.png\n");
    }
}

// --- INICIALIZAÇÃO DOS BUFFERS OPENGL ---
void initGLBuffers() {
    // Cria VAO e VBO para os vértices dos tiles/sprites
    float verts[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };
    unsigned int idxs[] = { 0, 1, 2, 0, 2, 3 };
    glGenVertexArrays(1, &tileVAO);
    glGenBuffers(1, &tileVBO);
    glBindVertexArray(tileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// --- FUNÇÃO DE DESENHO DE TILE ---
void drawTile(int tileIndex, int i, int j, glm::mat4 projection, bool darken = false) {
    // Desenha um tile na posição (i, j) com possível escurecimento
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture);
    glUniform1i(glGetUniformLocation(shaderCore, "tileset"), 0);
    if (darken) glUniform4f(glGetUniformLocation(shaderCore, "colorMod"), 0.5f, 0.5f, 0.5f, 1.0f);
    else        glUniform4f(glGetUniformLocation(shaderCore, "colorMod"), 1.0f, 1.0f, 1.0f, 1.0f);
    float tilesPerRow   = 7.0f;
    float tileU         = (float)tileIndex / tilesPerRow;
    float tileV         = 0.0f;
    float tileUW        = 1.0f / tilesPerRow;
    float tileVH        = 1.0f; 
    glm::mat4 model     = glm::mat4(1.0f);
    float screenX       = (j - i) * (BLOCK_WIDTH / 2.0f);
    float screenY       = (i + j) * (BLOCK_HEIGHT / 2.0f);
    model               = glm::translate(model, glm::vec3(screenX + WINDOW_WIDTH / 2 - BLOCK_WIDTH / 2, screenY + WINDOW_HEIGHT / 2 - (worldRows * BLOCK_HEIGHT) / 2, 0.0f));
    model               = glm::scale(model, glm::vec3(BLOCK_WIDTH, BLOCK_HEIGHT, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    float centerU = tileU + tileUW / 2.0f;
    float centerV = tileV + tileVH / 2.0f;
    float verts[] = {
        0.5f, 0.5f, centerU,        centerV,
        0.5f, 0.0f, centerU,        tileV,
        1.0f, 0.5f, tileU + tileUW, centerV,
        0.5f, 1.0f, centerU,        tileV + tileVH,
        0.0f, 0.5f, tileU,          centerV,
        0.5f, 0.0f, centerU,        tileV
    };
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glBindVertexArray(tileVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
}

// --- FUNÇÃO DE DESENHO DE MOEDA ---
void drawCoin(int i, int j, glm::mat4 projection) {
    // Desenha uma moeda animada na posição (i, j)
    int winW, winH;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &winW, &winH);
    float mapHeight = (worldRows + worldCols) * (BLOCK_HEIGHT / 2.0f);
    float offsetY   = winH / 2 - mapHeight / 2;
    glUniform4f(glGetUniformLocation(shaderCore, "colorMod"), 1.0f, 1.0f, 1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    float screenX   = (j - i) * (BLOCK_WIDTH / 2.0f);
    float screenY   = (i + j) * (BLOCK_HEIGHT / 2.0f);
    float px        = screenX + winW / 2 - BLOCK_WIDTH / 2;
    float py        = screenY + offsetY;
    float coinW     = BLOCK_WIDTH * 0.25f;
    float coinH     = BLOCK_HEIGHT * 0.35f;
    model           = glm::translate(model, glm::vec3(px + (BLOCK_WIDTH - coinW) / 2, py + (BLOCK_HEIGHT - coinH) / 2, 0.0f));
    model           = glm::scale(model, glm::vec3(coinW, coinH, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, coinSprites[coinAnimFrame]);
    glUniform1i(glGetUniformLocation(shaderCore, "tileset"), 0);
    float verts[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glBindVertexArray(tileVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// --- FUNÇÃO DE RESET DO JOGO ---
void resetGame() {
    // Reinicia o estado do jogo, reposiciona o jogador e conta as moedas
    loadMap("./assets/map.txt");
    heroCol     = (int)std::floor(worldCols / 2.0);
    heroRow     = (int)std::floor(worldRows / 2.0);
    if (heroRow >= 0 && heroRow < worldRows && heroCol >= 0 && heroCol < worldCols) {
        worldMap[heroRow][heroCol].tileIndex = 6;
        worldMap[heroRow][heroCol].hasCoin   = false;
    }
    coinsCollected   = 0;
    coinsTotal  = 0;
    for (int i = 0; i < worldRows; ++i) for (int j = 0; j < worldCols; ++j) if (worldMap[i][j].hasCoin) coinsTotal++;
    playState = RUNNING;
    gameStartTime = glfwGetTime();
}

// --- FUNÇÃO DE PROCESSAMENTO DE INPUT ---
void processInput(GLFWwindow* window) {
    // Processa teclas de movimento e reinício
    static double lastMove  = 0;
    double now              = glfwGetTime();
    if (playState != RUNNING) {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            resetGame();
            printf("----- Game iniciado! -----\n");
            printf("Objetivo: Coletar todas as moedas\nCair na lava e sera seu fim\n");
        }
        return;
    }
    if (now - lastMove < 0.20) return; 
    int dCol = 0, dRow = 0;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)      dRow--;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)    dRow++;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)    dCol--;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)   dCol++;
    int nextCol      = heroCol + dCol;
    int nextRow      = heroRow + dRow;
    int rows    = (int)worldMap.size();
    int cols    = rows > 0 ? (int)worldMap[0].size() : 0;
    if (nextCol >= 0 && nextCol < cols && nextRow >= 0 && nextRow < rows && (dCol != 0 || dRow != 0)) {
        if (worldMap[nextRow][nextCol].tileIndex == 5) return; 
        heroCol     = nextCol;
        heroRow     = nextRow;
        lastMove    = now;
        if (worldMap[heroRow][heroCol].hasCoin) {
            worldMap[heroRow][heroCol].hasCoin = false;
            coinsCollected++;
        }
        if (coinsCollected == coinsTotal) {
            playState = WON;
            double elapsed = now - gameStartTime;
            printf("Muito bem, voce ganhou!\n", elapsed);
            printf("Pressione R para reiniciar o game.\n\n");
        }
        if (worldMap[heroRow][heroCol].tileIndex == 3) {
            playState = GAMEOVER;
            printf("Decepcionante, voce pisou na lava!\n");
            printf("Pressione R para reiniciar o Game.\n\n");
        }
    }
}

// --- FUNÇÃO DE DESENHO DO JOGADOR ---
void drawPlayer(int i, int j, glm::mat4 projection) {
    // Desenha o sprite animado do jogador na posição (i, j)
    int winW, winH;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &winW, &winH);
    float mapHeight = (worldRows + worldCols) * (BLOCK_HEIGHT / 2.0f);
    float offsetY   = winH / 2 - mapHeight / 2;
    glUniform4f(glGetUniformLocation(shaderCore, "colorMod"), 1.0f, 1.0f, 1.0f, 1.0f);
    float screenX   = (j - i) * (BLOCK_WIDTH / 2.0f);
    float screenY   = (i + j) * (BLOCK_HEIGHT / 2.0f);
    float px        = screenX + winW / 2 - BLOCK_WIDTH / 2;
    float py        = screenY + offsetY;
    float spriteW   = BLOCK_WIDTH * 1.0f; 
    float spriteH   = BLOCK_HEIGHT * 2.0f; 
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(px + (BLOCK_WIDTH - spriteW) / 2 + 15, py + (BLOCK_HEIGHT - spriteH) - BLOCK_HEIGHT / 4, 0.0f));
    model = glm::scale(model, glm::vec3(spriteW, spriteH, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderCore, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    float fw = 1.0f / 4.0f, fh = 1.0f; 
    float u0 = idleAnimFrame * fw;
    float v0 = 0.0f;
    float u1 = u0 + fw;
    float v1 = v0 + fh;
    float verts[] = {
        0.0f, 0.0f, u0, v0,
        0.0f, 1.0f, u0, v1,
        1.0f, 1.0f, u1, v1,
        1.0f, 0.0f, u1, v0,
    };
    glBindBuffer(GL_ARRAY_BUFFER, tileVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    glBindVertexArray(tileVAO);
    glBindTexture(GL_TEXTURE_2D, idleTexture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// --- LOOP PRINCIPAL DO JOGO ---
int main() {
    // Inicializa GLFW, OpenGL, carrega recursos e executa o loop principal
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tilemap Isometrico", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaderCore = createShader();
    glUseProgram(shaderCore);
    resetGame();
    printf("--- Jogo iniciado! ---\n");
    printf("Colete todas as moedas, sem pisar na lava!\n");
    loadTilesetTexture(std::string("./assets/tilesets/") + atlasFile);
    loadCoinTextures();
    loadPlayerIdleTexture();
    initGLBuffers();
    orthoProj = glm::ortho(0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f);
    double lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now      = glfwGetTime();
        double delta    = now - lastFrameTime;
        lastFrameTime        = now;
        coinAnimElapsed += delta;
        if (coinAnimElapsed > coinAnimInterval) {
            coinAnimElapsed   = 0;
            coinAnimFrame       = (coinAnimFrame + 1) % 10;
        }
        idleAnimElapsed += delta;
        if (idleAnimElapsed > idleAnimInterval) {
            idleAnimElapsed = 0;
            idleAnimFrame = (idleAnimFrame + 1) % 4;
        }
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, atlasTexture);
        for (int i = 0; i < worldRows; ++i) for (int j = 0; j < worldCols; ++j) {
            bool darken = (heroRow == i && heroCol == j);
            drawTile(worldMap[i][j].tileIndex, i, j, orthoProj, darken);
            if (worldMap[i][j].hasCoin) drawCoin(i, j, orthoProj);
        }
        drawPlayer(heroRow, heroCol, orthoProj);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    printf("------------------------------------------\n");
    glfwTerminate();
    return 0;
}