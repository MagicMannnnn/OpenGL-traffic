#pragma once


#include <app_input.hpp>
#include <road_manager.hpp>
#include <car_manager.hpp>

class App {
public:
	App() = default;

	void run() {
        float currentFrame = 0.0f;
        float lastFrame = 0.0f;

        float timer = 0.f;
        int frameCounter = 0;

        m_carManager = CarManager();

        while (!glfwWindowShouldClose(m_window)) {

            currentFrame = static_cast<float>(glfwGetTime());
            Global::deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            Global::processInput(m_window);

            glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mainGameLoop();

            glfwSwapBuffers(m_window);
            glfwPollEvents();

        }
	}




private:
    void mainGameLoop() {
        render();

        if (Global::R) {
            m_roadManager.reGenerate();
        }
        if (Global::P) {
            m_roadManager.demoPath();
        }
    }


    void render() {
        m_roadManager.Render(m_projection, m_camera);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) std::cout << "GL error after road render: " << err << std::endl;


        m_carManager.Render(m_projection, m_camera);
        err = glGetError();
        if (err != GL_NO_ERROR) std::cout << "GL error after car render: " << err << std::endl;

        

    }




private:
    Camera& m_camera = Global::camera;
    GLFWwindow*& m_window = Global::window;
    glm::mat4 m_projection = glm::perspective(glm::radians(m_camera.Zoom), (float)Settings::SCR_WIDTH / (float)Settings::SCR_HEIGHT, 0.1f, 500.0f);
    RoadManager m_roadManager;
    CarManager m_carManager;
};