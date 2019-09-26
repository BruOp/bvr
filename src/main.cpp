#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

namespace bvr
{
    inline void debugLog(const char* output)
    {
#ifndef NDEBUG
        std::cout << "INFO: " << output << std::endl;
#endif
    }


    struct RenderConfig
    {
        int width = 1280;
        int height = 720;
        std::vector <char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };


    struct QueueFamilyIndices
    {
        uint32_t graphicsFamily = UINT32_MAX;

        bool isComplete()
        {
            return graphicsFamily != UINT32_MAX;
        }
    };


    class Renderer
    {

    public:
        Renderer() = default;
        Renderer(const RenderConfig& config, GLFWwindow* window) :
            m_config(config),
            m_window(window)
        { };

        ~Renderer()
        {
            debugLog("Cleaning up Renderer!");

            destroyDebugMessenger();
            m_instance.destroy();
        }

        void init()
        {
            initVulkan();
            debugLog("Renderer Initialized!");
        }

        void renderFrame() { };

    private:
        void initVulkan()
        {
            createInstance();
            setupDebugMessenger();
            pickPhysicalDevice();
        }

        void createInstance()
        {
            vk::ApplicationInfo appInfo{
                "BVR",
                VK_MAKE_VERSION(0, 1, 0),
                "N/A",
                VK_MAKE_VERSION(0, 1, 0),
                VK_API_VERSION_1_1,
            };


            std::vector<const char*> requiredExtensions{ getRequiredExtensions() };
            validateExtensions(requiredExtensions);


            if (!checkValidationLayerSupport()) {
                throw std::runtime_error("Validation requested, but layers not available.");
            };

            vk::InstanceCreateInfo createInfo{
                vk::InstanceCreateFlags(),
                &appInfo,
                static_cast<uint32_t>(m_config.validationLayers.size()),
                m_config.validationLayers.data(),
                static_cast<uint32_t>(requiredExtensions.size()),
                requiredExtensions.data(),
            };

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

            if (isValidationEnabled()) {
                debugCreateInfo = getDebugMessengerCreateInfo();
                createInfo.pNext = &debugCreateInfo;
            }

            m_instance = vk::createInstance(createInfo);

        }

        bool isValidationEnabled()
        {
            return m_config.validationLayers.size();
        }

        std::vector<const char*> getRequiredExtensions() const
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (m_config.validationLayers.size() > 0) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

        void validateExtensions(std::vector<const char*> requiredExtensions) const
        {
            std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

            for (size_t i = 0; i < requiredExtensions.size(); ++i) {
                const char* requiredExtension = requiredExtensions[i];
                bool requirementMet = false;

                for (size_t j = 0; j < extensions.size(); j++) {
                    if (strcmp(extensions[j].extensionName, requiredExtension) == 0) {
                        requirementMet = true;
                        break;
                    }
                }

                if (!requirementMet) {
                    std::string errorString{ requiredExtension };
                    throw std::runtime_error(errorString.append(" is not supported!"));
                }
            }
        }

        bool checkValidationLayerSupport() const
        {
            std::vector < vk::LayerProperties> layers = vk::enumerateInstanceLayerProperties();

            for (const char* layerName : m_config.validationLayers) {
                bool layerFound = false;

                for (const vk::LayerProperties& layerProps : layers) {
                    if (strcmp(layerProps.layerName, layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }

            return true;
        }

        void pickPhysicalDevice()
        {
            std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();

            for (const auto& device : devices) {
                if (isDeviceSuitable(device)) {
                    m_physicalDevice = device;
                    break;
                }
            }

            if (!m_physicalDevice) {
                throw std::runtime_error("Failed to find suitable physical device!");
            }

#ifndef  NDEBUG
            vk::PhysicalDeviceProperties props = m_physicalDevice.getProperties();
            std::string message = "Physical Device found! ";
            debugLog(message.append(props.deviceName).c_str());
#endif
        }

        bool isDeviceSuitable(const vk::PhysicalDevice& device) const
        {
            QueueFamilyIndices indices = findQueueFamilies(device);

            vk::PhysicalDeviceProperties props = device.getProperties();
            vk::PhysicalDeviceFeatures features = device.getFeatures();

            return indices.isComplete() && props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        }

        QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& physicalDevice) const
        {
            QueueFamilyIndices indices;

            std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
            int i = 0;
            for (const auto& queueFamily : queueFamilies) {
                if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
                    indices.graphicsFamily = i;
                }

                if (indices.isComplete()) {
                    return indices;
                }
                ++i;
            }

            return indices;
        }

        VkDebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo() const
        {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo{
                {},
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                debugCallback,
                nullptr,
            };

            return createInfo;
        }

        void setupDebugMessenger()
        {
            if (m_config.validationLayers.size() == 0) {
                return;
            }

            VkDebugUtilsMessengerCreateInfoEXT createInfo = getDebugMessengerCreateInfo();

            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)m_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT");

            VkDebugUtilsMessengerEXT dbgMessenger;
            if (func != nullptr) {
                func(m_instance, &createInfo, nullptr, &dbgMessenger);
                m_dbgMessenger = dbgMessenger;
            }
            else {
                throw std::runtime_error("Failed to setup the Debug Messenger");
            }
        }

        void destroyDebugMessenger()
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)m_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr && m_dbgMessenger) {
                func(m_instance, m_dbgMessenger, nullptr);
            }
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {

            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        RenderConfig m_config{};
        GLFWwindow* m_window = nullptr;

        vk::Instance m_instance{};
        vk::DebugUtilsMessengerEXT m_dbgMessenger;
        vk::PhysicalDevice m_physicalDevice;
    };

    class BVRApp
    {
    public:
        BVRApp(const RenderConfig& config) : m_config(config)
        { }

        ~BVRApp()
        {
            debugLog("Shutting Down!");
            if (m_window != nullptr) {
                glfwDestroyWindow(m_window);
            }
            if (!m_glfwInitialized) {
                return;
            }

            glfwTerminate();
        }

        void run()
        {
            initWindow(m_config.width, m_config.height);
            initRenderer();
            mainLoop();

        }

        BVRApp(const BVRApp& app) = delete;
        BVRApp& operator=(const BVRApp& app) = delete;
        BVRApp(BVRApp&& app) = delete;
        BVRApp& operator=(BVRApp&& app) = delete;

    private:

        void initWindow(int width, int height)
        {
            m_glfwInitialized = glfwInit();
            if (!m_glfwInitialized) {
                throw std::runtime_error("Could not initialize GLFW");
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            m_window = glfwCreateWindow(width, height, "BVR", nullptr, nullptr);

            if (m_window == nullptr) {
                throw std::runtime_error("Could not create GLFW window!");
            }
        }

        void initRenderer()
        {
            m_renderer = Renderer{ m_config, m_window };
            m_renderer.init();
        }

        void mainLoop()
        {
            debugLog("Entering Main Loop!");
            while (!glfwWindowShouldClose(m_window)) {
                glfwPollEvents();
            }
        }

        RenderConfig m_config;
        bool m_glfwInitialized = true;
        GLFWwindow* m_window = nullptr;

        Renderer m_renderer;

    };
}


int main()
{
    bvr::RenderConfig config{
        1280,
        720,
    };

#ifdef NDEBUG
    config.validationLayers = { };
#endif

    bvr::BVRApp app{ config };

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
