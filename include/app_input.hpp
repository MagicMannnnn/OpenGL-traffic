#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Settings.hpp>
#include <iostream>
#include <camera.hpp>
//#include <textRenderer.hpp>

namespace Global {
    

    GLFWwindow* window;
    Camera camera(glm::vec3(0.0f, 10.f, 0.f));
    float mouseX = Settings::SCR_WIDTH / 2.0f;
    float mouseY = Settings::SCR_HEIGHT / 2.0f;
    float mouseLastX = Settings::SCR_WIDTH / 2.0f;
    float mouseLastY = Settings::SCR_HEIGHT / 2.0f;
    float mouseDeltaX;
    float mouseDeltaY;
    bool firstMouse = true;

    bool leftClick = false;
    bool rightClick = false;
    bool firstLeftClick = true;

    unsigned int codepoint;
    bool typing = false;





    float deltaTime = 0.0f;




    void resetInputs() {
        
    }


    void processInput(GLFWwindow* window)
    {



        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.setFastVelocity(true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);

        camera.setFastVelocity(false);






        



    }


    void characterCallback(GLFWwindow* window, unsigned int _codepoint) {
        codepoint = _codepoint;
        typing = true;
        
    }


    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        Settings::SCR_HEIGHT = height;
        Settings::SCR_WIDTH = width;
        glViewport(0, 0, width, height);
    }


    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        mouseLastX = mouseX;
        mouseLastY = mouseY;
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        mouseX = xpos;
        mouseY = ypos;


        float xoffset = mouseX - mouseLastX;
        float yoffset = mouseLastY - mouseY; // reversed since y-coordinates go from bottom to top
        mouseDeltaX = xoffset;
        mouseDeltaY = yoffset;


        if (rightClick) {
            camera.ProcessMouseMovement(xoffset, yoffset);
        }
        

    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {

        camera.ProcessMouseScroll(static_cast<float>(yoffset), deltaTime);
    }


    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                if (firstMouse) {
                    leftClick = true;
                    glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                firstMouse = false;
                
            }
            if (action == GLFW_RELEASE) {
                firstMouse = true;
                leftClick = false;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && !leftClick) {
            if (action == GLFW_PRESS) {
                rightClick = true;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            if (action == GLFW_RELEASE) {
                rightClick = false;
                glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

