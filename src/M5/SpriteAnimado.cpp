#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Sprite.h"
#include <iostream>

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
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Soldado Sprite", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Falha ao criar janela\n";
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f); // cor de fundo azul escuro

    Sprite spriteParadoFrente("src/M5/assets/soldier/Stop/StopFoward.png");
    Sprite spriteParadoTras("src/M5/assets/soldier/Stop/StopBack.png");
    Sprite spriteCorrendoFrente("src/M5/assets/soldier/Run/RunFoward.png");
    Sprite spriteCorrendoTras("src/M5/assets/soldier/Run/RunBack.png");
    Sprite spriteAbaixadoFrente("src/M5/assets/soldier/Down/DownFoward.png");
    Sprite spriteAbaixadoTras("src/M5/assets/soldier/Down/DownBack.png");

    Estado estadoAtual = PARADO_FRENTE;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

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
                spriteParadoFrente.draw(350, 250);
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
