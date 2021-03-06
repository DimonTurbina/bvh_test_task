/* Renderer.cpp
 * Author : Kozhukharov Nikita
 */


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Renderer/Renderer.h>
#include <Renderer/Shaders.h>


static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/***
 * WindowHandler methods definition
 ***/
void GLRenderer::WindowHandler::makeContextCurrent() {
    glfwMakeContextCurrent(window);
}

GLRenderer::WindowHandler::WindowHandler(int w, int h, const char* window_name) 
    : width(w), height(h) {
    window = glfwCreateWindow(w, h, window_name, NULL, NULL);
    if (windowReady()) {
        makeContextCurrent();
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        camera.recountProjMatrix(45.0, getWindowAspect(), 0.1, 100.0);
    }
}

float GLRenderer::WindowHandler::getWindowAspect() {
    return (float)width / height;
}

void GLRenderer::WindowHandler::updateTime() {
    float curTime = (double)glfwGetTime();
    deltaTime = curTime - prevGlobalTime;
    prevGlobalTime = curTime;
}

void GLRenderer::WindowHandler::updateFramebufferSize() {
    int newWidth, newHeight;
    glfwGetWindowSize(window, &newWidth, &newHeight);

    if (newWidth != width || newHeight != height) {
        width = newWidth;
        height = newHeight;
        camera.recountProjMatrix(45.0, getWindowAspect(), 0.1, 100.0);
    }
}

void GLRenderer::WindowHandler::swapBuffers() {
    glfwSwapBuffers(window);
}

/***
 * Keys processing function
 ***/
void GLRenderer::WindowHandler::processInput() {
    static bool ctrlPressed = false;
    static bool mouseBLeftPressed = isButtonPressed(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    // static bool mouseBRightPressed;// = glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
    if (isKeyPressed(GLFW_KEY_W))
        camera.moveByKeys(Camera::CameraDirection::FORWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_A))
        camera.moveByKeys(Camera::CameraDirection::LEFT, deltaTime);
    if (isKeyPressed(GLFW_KEY_D))
        camera.moveByKeys(Camera::CameraDirection::RIGHT, deltaTime);
    if (isKeyPressed(GLFW_KEY_S))
        camera.moveByKeys(Camera::CameraDirection::BACKWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_C)) /// if debug
        camera.printCameraSettings();
    if (isKeyPressed(GLFW_KEY_KP_ADD))
        camera.changeStreifSpeed(1, deltaTime);
    else if (isKeyPressed(GLFW_KEY_KP_SUBTRACT))
        camera.changeStreifSpeed(-1, deltaTime);
    
    if (mouseBLeftPressed && !isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { // LMB released
        mouseBLeftPressed = false;
        lastX = (float)width / 2;
        lastY = (float)height / 2;
        glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else if (!mouseBLeftPressed && isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { // LMB pressed
        mouseBLeftPressed = true;
        lastX = (float)width / 2;
        lastY = (float)height / 2;
        glfwSetCursorPos(window, (double)width / 2, (double)height / 2);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    camera.moveByMouse((float)cursorX - lastX, (float)cursorY - lastY, mouseBLeftPressed);
    lastX = (float)cursorX;
    lastY = (float)cursorY;
}

bool GLRenderer::WindowHandler::shouldClose() {
    return glfwWindowShouldClose(window);
}

bool GLRenderer::WindowHandler::windowReady() {
    return window != NULL;
}

bool GLRenderer::WindowHandler::isKeyPressed(int button) {
    return glfwGetKey(window, button) == GLFW_PRESS;
}

bool GLRenderer::WindowHandler::isButtonPressed(int button) {
    return glfwGetMouseButton(window, button);
}


/***
 * GLRenderer methods definition
 ***/
bool GLRenderer::initGL(int w, int h, const char* window_name) {
    if (!glfwInit())
        return false;           

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    windowHandler = new WindowHandler(w, h, window_name);
    if (!windowHandler->windowReady()) {
        glfwTerminate();
        return false;
    }
    return true;
}

bool GLRenderer::initGLLoader() {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return true;
    //std::cerr << "Failed to get GL-functions addresses.\n";
    return false;
}

int GLRenderer::addShaderProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {
    ShaderProgram * p = new ShaderProgram(vertexShaderFile, fragmentShaderFile);

    if (!p->isValid()) {
        delete p;
        return -1;
    }
    shaderPrograms.emplace_back(p);
    return shaderPrograms.size() - 1;
}

int GLRenderer::addMesh(Mesh * m) {
    if (!m)
        return -1;
    meshPtrs.emplace_back(m);
    return meshPtrs.size() - 1;
}

GLRenderer::GLRenderer(int w, int h, const char* window_name) {
    renderReady = initGL(w, h, window_name) && initGLLoader();
    glEnable(GL_DEPTH_TEST);
}


void GLRenderer::startDrawLoop() {
    if (!renderReady) {
        //std::cerr << "not ready to render\n";
        return;
    }

    while (!windowHandler->shouldClose()) {
        windowHandler->updateTime();
        windowHandler->updateFramebufferSize();
        windowHandler->processInput();
        // clear window
        glClearColor(0.25f, 0.25f, 0.25f, 0.5f);               // setting the color, can be called once
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // cleaning the color buffer, necessary to call every time

        // send data to shaders
        for (const auto& shader : shaderPrograms) {
            shader->setMat4(windowHandler->camera.getProjMatrix(), std::string("proj"));
            shader->setMat4(windowHandler->camera.getViewMatrix(), std::string("view"));
            glm::vec3 defaultLight(-20.0, 27.0, 40.0);
            shader->setVec3(defaultLight, std::string("lightPos"));
        }

        for (const auto& m : meshPtrs) {
            m->draw(*shaderPrograms[m->getShaderId()]);
        }

        windowHandler->swapBuffers();
        glfwPollEvents();
    }
}

void GLRenderer::setCameraPos(glm::vec3 camPos) {
    windowHandler->camera.setStartPos(camPos);
}

GLRenderer::~GLRenderer() {
    glfwTerminate();
}


/***
 * Window size change callback
 ***/

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
