#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #define _USE_MATH_DEFINES
#include <cmath>

#include "Tree.h"
#include "shaders.h"


const unsigned int SRC_WIDTH = 1024;
const unsigned int SRC_HEIGHT = 720;

float scaling (0.01f);
float height (0.0f);


void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void proccessInput(GLFWwindow * window);

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "lab1", NULL, NULL);
    if (window == NULL){
        std::cout<<"Failed to create GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"Failed to initialize GLAD"<<std::endl;
        return -1;
    }
    
    

    Shader branchShaderProgram(".\\shaders\\vshader.vs", ".\\shaders\\fshader.fs");
    Shader leafShaderProgram(".\\shaders\\vshader.vs", ".\\shaders\\leafShader.fs");
    

    glm::mat4 projection = glm::mat4(1.0f);

    
    branchShaderProgram.Use();
    branchShaderProgram.SetMat4("projection", projection);
    
    
    leafShaderProgram.Use();
    leafShaderProgram.SetMat4("projection", projection);

   
    Tree tree("X");

    tree.MakeAxiom(5);
    tree.MakeBranchesCoordsArray();
    tree.MakeLeavesCoordsArray();
    tree.DrawTree(tree.axiom, glm::vec2(0.0f, 0.0f), 90);

    // Make array of offsets to place trees in different positions
    unsigned int amount(100);
    glm::vec2* modelOffsets = new glm::vec2[amount];
    
    std::srand(static_cast<unsigned int>(glfwGetTime()));
    for (int i(0); i < amount; ++i) {
        float x = rand() % 500 - 250;
        float y = rand() % 500 - 250;
        modelOffsets[i] = glm::vec2(x, y);
    }

    // Defining instance buffer
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * 2 * sizeof(float), &modelOffsets[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    unsigned int branchVBO, branchVAO, leafVBO, leafVAO;

    glGenVertexArrays(1, &branchVAO);
    glGenVertexArrays(1, &leafVAO);
    glGenBuffers(1, &branchVBO);

    // Binding Buffer for branches
    glBindVertexArray(branchVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, branchVBO);
    glBufferData(GL_ARRAY_BUFFER, tree.numOfBranches * 2 * 2 * sizeof(float), &tree.branchesCoords[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Binding Buffer for leaves
    glBindVertexArray(leafVAO);
    glGenBuffers(1, &leafVBO);

    glBindBuffer(GL_ARRAY_BUFFER, leafVBO);
    glBufferData(GL_ARRAY_BUFFER, tree.numOfLeaves * 6 * 2 * sizeof(float), &tree.leavesCoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)){
        proccessInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Creating model matrix to make wind
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(3*std::sin(0.5f * static_cast<float>(glfwGetTime())) + 2.f), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0, height, 0));
        view = glm::scale(view, glm::vec3(scaling, scaling, scaling));
        

        // Drawing branches
        glUseProgram(branchShaderProgram.ID);
        branchShaderProgram.SetMat4("model", model);
        branchShaderProgram.SetMat4("view", view);
        glBindVertexArray(branchVAO);
        glDrawArraysInstanced(GL_LINES, 0, tree.numOfBranches * 2, amount);
        glBindVertexArray(0);

        // Drawing leaves
        glUseProgram(leafShaderProgram.ID);
        leafShaderProgram.SetMat4("model", model);   
        leafShaderProgram.SetMat4("view", view);
        glBindVertexArray(leafVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, tree.numOfLeaves * 6, amount);

        glBindVertexArray(0);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &leafVBO);
    glDeleteBuffers(1, &branchVBO);
    glDeleteVertexArrays(1, &leafVAO);
    glDeleteVertexArrays(1, &branchVAO);


    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height){
    glViewport(0, 0, width, height);
}

void proccessInput(GLFWwindow * window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        scaling += 0.001f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        scaling -= 0.001f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        height -= 0.05f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        height += 0.05f;
    }
}