#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <chrono>

enum Estado {
    PARADO_FRENTE,
    PARADO_TRAS,
    CORRENDO_FRENTE,
    CORRENDO_TRAS,
    ABAIXADO_FRENTE,
    ABAIXADO_TRAS
};

GLFWwindow* g_window = nullptr;
int g_gl_width = 800;
int g_gl_height = 600;

int main() {
    std::cout << "Iniciando programa SpriteAnimado!" << std::endl;

    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Soldado Sprite", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Falha ao criar janela\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD\n";
        return -1;
    }
    glViewport(0, 0, g_gl_width, g_gl_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);

    Sprite::InitGLResources();
    // Projeção ortográfica padrão OpenGL (origem canto inferior esquerdo)
    glm::mat4 proj = glm::ortho(0.0f, float(g_gl_width), 0.0f, float(g_gl_height), -1.0f, 1.0f);
    Sprite::SetProjection(proj);

    Sprite spriteParadoFrente("src/M5/assets/soldier/Stop/StopFoward.png");
    Sprite spriteParadoTras("src/M5/assets/soldier/Stop/StopBack.png");
    Sprite spriteCorrendoFrente("src/M5/assets/soldier/Run/RunFoward.png");
    Sprite spriteCorrendoTras("src/M5/assets/soldier/Run/RunBack.png");
    Sprite spriteAbaixadoFrente("src/M5/assets/soldier/Down/DownFoward.png");
    Sprite spriteAbaixadoTras("src/M5/assets/soldier/Down/DownBack.png");

    Estado estadoAtual = PARADO_FRENTE;

    std::cout << "Antes do loop principal" << std::endl;
    while (!glfwWindowShouldClose(window)) {
        std::cout << "Loop principal ativo" << std::endl;
        glClear(GL_COLOR_BUFFER_BIT);
        std::cout << "Antes do switch/case" << std::endl;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            estadoAtual = CORRENDO_FRENTE;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            estadoAtual = CORRENDO_TRAS;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (estadoAtual == CORRENDO_FRENTE || estadoAtual == PARADO_FRENTE)
                estadoAtual = ABAIXADO_FRENTE;
            else
                estadoAtual = ABAIXADO_TRAS;
        }
        else {
            if (estadoAtual == CORRENDO_FRENTE || estadoAtual == ABAIXADO_FRENTE)
                estadoAtual = PARADO_FRENTE;
            else if (estadoAtual == CORRENDO_TRAS || estadoAtual == ABAIXADO_TRAS)
                estadoAtual = PARADO_TRAS;
        }

        switch (estadoAtual) {
            case PARADO_FRENTE:
                spriteParadoFrente.draw(0, 0); // Teste: desenhar no canto inferior esquerdo
                break;
            case PARADO_TRAS:
                spriteParadoTras.draw(350, 250);
                break;
            case CORRENDO_FRENTE:
                spriteCorrendoFrente.draw(350, 250);
                break;
            case CORRENDO_TRAS:
                spriteCorrendoTras.draw(350, 250);
                break;
            case ABAIXADO_FRENTE:
                spriteAbaixadoFrente.draw(350, 250);
                break;
            case ABAIXADO_TRAS:
                spriteAbaixadoTras.draw(350, 250);
                break;
        }

        std::cout << "Teste: desenhando triângulo legacy" << std::endl;
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    std::cout << "Depois do loop principal" << std::endl;

    glfwTerminate();
    return 0;
}
