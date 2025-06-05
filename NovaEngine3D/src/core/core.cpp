// core library entrypoint
#include "core.hpp"
#include "core/renderer/renderer.hpp"
#include <memory>
#include <print>
#include <stdexcept>
#include <vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Nova::Core {
bool Application::initializeGLFW() {
  if (!glfwInit()) {
    throw std::runtime_error("Unable to initialize GLFW!");
    return false;
  }
  return true;
};

bool Application::createApplicationWindow() {
  if (isWindowResizable) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  } else {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  applicationWindow = glfwCreateWindow(initialApplicationWindowWidth,
                                       initialApplicationWindowHeight,
                                       applicationTitle, nullptr, nullptr);
  if (!applicationWindow) {
    throw std::runtime_error("Unable to create application window!");
    return false;
  }

  renderer = std::make_unique<Graphics::Renderer>(
      std::make_shared<GLFWwindow *>(applicationWindow));

  return true;
};

void Application::initializeVulkan() {
  renderer->createInstance();
  renderer->createSurface();
  renderer->selectPhysicalDevice();
  renderer->createLogicalDevice();
};

void Application::startApplicationRuntime() {
  while (!glfwWindowShouldClose(applicationWindow)) {
    glfwPollEvents();
  }
};

Application::Application(const char *title, int windowWidth, int windowHeight,
                         bool isWindowResizable)
    : applicationTitle(title), initialApplicationWindowWidth(windowWidth),
      initialApplicationWindowHeight(windowHeight),
      isWindowResizable(isWindowResizable) {
  initializeGLFW();
  createApplicationWindow();
  initializeVulkan();
  startApplicationRuntime();
};

Application::~Application() {
  glfwDestroyWindow(applicationWindow);
  glfwTerminate();
};
} // namespace Nova::Core