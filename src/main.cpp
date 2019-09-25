#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>


namespace bvr
{
    class HelloTriangleApplication
    {
    public:
        void run()
        {
            constexpr int WIDTH = 1280;
            constexpr int HEIGHT = 720;

            initWindow(WIDTH, HEIGHT);
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        void initWindow(int width, int height)
        {
            
            if (!glfwInit()) {
                throw std::runtime_error("Could not initialize GLFW");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            m_window = glfwCreateWindow(width, height, "BVR", nullptr, nullptr);

            if (m_window == nullptr) {
                throw std::runtime_error("Could not create GLFW window!");
            }
        }

        void initVulkan()
        {
            createInstance();
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(m_window)) {
                glfwPollEvents();
            }
        }

        void cleanup()
        {
            m_instance.destroy();
            glfwDestroyWindow(m_window);
            glfwTerminate();
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

        vk::Instance m_instance{};
        GLFWwindow* m_window = nullptr;

    };
}


int main()
{
    bvr::HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}