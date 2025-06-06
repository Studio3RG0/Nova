#include "renderer.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <print>

namespace Nova::Core::Graphics {

Renderer::Renderer(std::shared_ptr<GLFWwindow *> window) {
  appWindow = window;
};

Renderer::~Renderer() { device->waitIdle(); };

void Renderer::createInstance() {
  context.emplace(vkGetInstanceProcAddr);

  auto const vulkanVersion{context->enumerateInstanceVersion()};

  if (glfwVulkanSupported()) {
    std::println("Supported API Version: Vulkan {}.{}",
                 vk::apiVersionMajor(vulkanVersion),
                 vk::apiVersionMinor(vulkanVersion));
  }

  vk::ApplicationInfo applicationInfo{};
  applicationInfo.pApplicationName = "LumaEditor";
  applicationInfo.setApplicationVersion(vk::makeApiVersion(0, 0, 1, 0));
  applicationInfo.setPEngineName("NovaEngine3D");
  applicationInfo.setEngineVersion(vk::makeApiVersion(0, 1, 1, 0));
  applicationInfo.setApiVersion(vk::makeApiVersion(0, 1, 2, 0));

  std::vector<const char *> enabledExtensions{};
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    enabledExtensions.emplace_back(glfwExtensions[i]);
  };

  vk::InstanceCreateInfo instanceCreateInfo{};
#ifdef OS_APPLE
  enabledExtensions.emplace_back(
      vk::KHRPortabilityEnumerationExtensionName); // Only for MacOS
  instanceCreateInfo.setFlags(
      vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR); // Only for MacOS
#endif

  instanceCreateInfo.setPApplicationInfo(&applicationInfo);
  instanceCreateInfo.setPEnabledExtensionNames(*enabledExtensions.data());
  instanceCreateInfo.setEnabledExtensionCount(enabledExtensions.size());
  instanceCreateInfo.setEnabledLayerCount(0);

  instance = context->createInstance(instanceCreateInfo);
}

void Renderer::createSurface() {
  VkSurfaceKHR rawSurface;
  VkResult result =
      glfwCreateWindowSurface(**instance, *appWindow, nullptr, &rawSurface);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface!");
  }
  surface.emplace(*instance, rawSurface);
}

QueueFamilyIndices Renderer::findQueueFamilies(vk::PhysicalDevice device) {
  QueueFamilyIndices indices;

  auto const queueFamilies = device.getQueueFamilyProperties2();

  int i = 0;
  vk::Bool32 presentSupport = false;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFamilyProperties.queueFlags &
        vk::QueueFlagBits::eGraphics) {
      indices.graphicsFamily = i;
    }

    presentSupport = device.getSurfaceSupportKHR(i, *surface);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

SwapChainSupportDetails
Renderer::querySwapChainSupport(vk::PhysicalDevice device) {
  SwapChainSupportDetails details;

  auto _ = device.getSurfaceCapabilitiesKHR(*surface, &details.capabilites);
  auto supportedFormats = device.getSurfaceFormatsKHR(*surface);
  auto supportedPresentModes = device.getSurfacePresentModesKHR(*surface);

  if (supportedFormats.size() != 0) {
    details.formats.resize(supportedFormats.size());
    details.formats = supportedFormats;
  }

  if (supportedPresentModes.size() != 0) {
    details.formats.resize(supportedPresentModes.size());
    details.presentModes = supportedPresentModes;
  }

  return details;
};

bool Renderer::isDeviceSuitable(vk::PhysicalDevice device) {

  //   auto const deviceProperties = device.getProperties2().properties;
  //   auto const deviceFeatures = device.getFeatures2().features;

  //   return deviceProperties.deviceType ==
  //   vk::PhysicalDeviceType::eDiscreteGpu &&
  //          deviceFeatures.geometryShader;

  QueueFamilyIndices indices = findQueueFamilies(device);
  bool extensionsSupported = true; // Hard-coded for now
  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
};

void Renderer::selectPhysicalDevice() {
  auto const devices = instance->enumeratePhysicalDevices();

  if (devices.size() == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  }

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice.emplace(*instance, *device);
      break;
    }
  }
  if (physicalDevice == nullptr) {
    throw std::runtime_error("Failed to find suitable GPU!");
  }

  auto const deviceName =
      physicalDevice->getProperties2().properties.deviceName;
  auto const deviceApiVersion =
      physicalDevice->getProperties2().properties.apiVersion;

  std::println("Selected Device: {}", deviceName.data());
  std::println("Selected Device API Version: Vulkan {}.{}",
               vk::apiVersionMajor(deviceApiVersion),
               vk::apiVersionMinor(deviceApiVersion));
}

void Renderer::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(*physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    vk::DeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.setQueueFamilyIndex(queueFamily);
    queueCreateInfo.setQueueCount(1); // Hard-coded for now;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice->getFeatures();

  std::vector<const char *> enabledExtensions{};

  enabledExtensions.emplace_back(vk::KHRSwapchainExtensionName);

#ifdef OS_APPLE
  enabledExtensions.emplace_back("VK_KHR_portability_subset"); // only for MacOS
#endif

  vk::DeviceCreateInfo createInfo{};
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = queueCreateInfos.size();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.setPEnabledExtensionNames(*enabledExtensions.data());
  createInfo.setEnabledExtensionCount(enabledExtensions.size());

  device.emplace(*physicalDevice, createInfo);
  graphicsQueue.emplace(*device, indices.graphicsFamily.value(), 0);
  presentQueue.emplace(*device, indices.presentFamily.value(), 0);
};

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
        availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return availableFormat;
    }
  }
  return availableFormats[0];
};

vk::PresentModeKHR Renderer::chooseSwapPresentMode(
    const std::vector<vk::PresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    }
  }
  return vk::PresentModeKHR::eFifo;
}

vk::Extent2D
Renderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilites) {
  if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilites.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(*appWindow, &width, &height);

    vk::Extent2D actualExtent = {static_cast<uint32_t>(width),
                                 static_cast<uint32_t>(height)};
    actualExtent.width =
        std::clamp(actualExtent.width, capabilites.maxImageExtent.width,
                   capabilites.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilites.minImageExtent.height,
                   capabilites.maxImageExtent.height);

    return actualExtent;
  }
}

} // namespace Nova::Core::Graphics