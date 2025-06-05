#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0

#include "vulkan/vulkan_raii.hpp"
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Nova::Core::Graphics {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilites;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
};

class Renderer {
public:
  Renderer(std::shared_ptr<GLFWwindow *> window);
  ~Renderer();

public:
  void createInstance();
  void createSurface();
  bool isDeviceSuitable(vk::PhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
  void selectPhysicalDevice();
  void createLogicalDevice();
  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats);
  vk::PresentModeKHR chooseSwapPresentMode(
      const std::vector<vk::PresentModeKHR> &availablePresentModes);
  vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilites);

public:
  std::shared_ptr<GLFWwindow *> appWindow;

private:
  std::optional<vk::raii::Context> context{};
  std::optional<vk::raii::Instance> instance{};
  std::optional<vk::raii::SurfaceKHR> surface{};

  std::optional<vk::raii::PhysicalDevice> physicalDevice{};
  std::optional<vk::raii::Device> device{};

  std::optional<vk::raii::Queue> graphicsQueue{};
  std::optional<vk::raii::Queue> presentQueue{};
};

} // namespace Nova::Core::Graphics