#pragma once

#ifdef OS_WINDOWS
#define DllExport __declspec(dllexport)
#else
#define DllExport
#endif
#include "renderer/renderer.hpp"
#include <memory>

namespace Nova::Core {

class DllExport Application {
public:
  Application(const char *title, int windowWidth, int windowHeight,
              bool isWindowResizable);
  ~Application();

  GLFWwindow *applicationWindow = nullptr;
  const char *applicationTitle = "Untitled";
  int initialApplicationWindowWidth = 640;
  int initialApplicationWindowHeight = 480;
  bool isWindowResizable = false;

private:
  bool initializeGLFW();
  bool createApplicationWindow();
  void initializeVulkan();
  void startApplicationRuntime();

  std::unique_ptr<Graphics::Renderer> renderer = nullptr;
};

} // namespace Nova::Core