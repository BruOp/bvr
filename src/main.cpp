#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

namespace bvr
{
    struct RenderConfig
    {
        int width = 1280;
        int height = 720;
        std::vector <vk::ValidationFlagsEXT> validationLayers = {};
    };
    

    class Renderer
    {

    public:
        Renderer() = default;
        Renderer(const RenderConfig& config, GLFWwindow* window) :
            m_config(config),
            m_window(window)
        { };

        void init()
        {
            initVulkan();
        }

        void cleanup()
        {
            m_instance.destroy();
        }

        void renderFrame() { };

    private:
        void initVulkan()
        {
            createInstance();
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


            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            validateExtensions(glfwExtensions, glfwExtensionCount);



            vk::InstanceCreateInfo createInfo{
                vk::InstanceCreateFlags(),
                &appInfo,
                0,
                0,
                glfwExtensionCount,
                glfwExtensions,
            };

            m_instance = vk::createInstance(createInfo);
        }

        void validateExtensions(const char** requiredExtensions, uint32_t requiredExtensionCount) const
        {
            std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

            for (uint32_t i = 0; i < requiredExtensionCount; ++i) {
                const char* requiredExtension = requiredExtensions[i];
                bool requirementMet = false;

                for (size_t j = 0; j < extensions.size(); j++) {
                    if (strcmp(extensions[j].extensionName, requiredExtension)) {
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

        RenderConfig m_config{};
        GLFWwindow* m_window = nullptr;

        vk::Instance m_instance{};
    };

    class BVRApp
    {
    public:
        BVRApp(const RenderConfig& config) : m_config(config)
        { }

        ~BVRApp()
        {
            if (m_window != nullptr) {
                glfwDestroyWindow(m_window);
            }
            if (!m_glfwInitialized) {
                return;
            }

            glfwTerminate();
            m_renderer.cleanup();
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
        }

        void mainLoop()
        {
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
        { },
    };

    bvr::BVRApp app{config};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}