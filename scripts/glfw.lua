GLFW_DIR = path.join(EXTERNAL_DIR, "glfw")
GLFW_SRC_DIR = path.join(GLFW_DIR, "src")
GLFW_INCLUDE_DIR = path.join(GLFW_DIR, "include")

project("GLFW")
uuid(os.uuid("GLFW"))
kind "StaticLib"

defines {
  "WIN32",
  "_WINDOWS",
  "_GLFW_USE_CONFIG_H",
  "_CRT_SECURE_NO_WARNINGS",
}

CMAKE_INTDIR="Debug"

files {
  path.join(GLFW_SRC_DIR, "context.c"),
  path.join(GLFW_SRC_DIR, "egl_context.c"),
  path.join(GLFW_SRC_DIR, "inti.c"),
  path.join(GLFW_SRC_DIR, "input.c"),
  path.join(GLFW_SRC_DIR, "monitor.c"),
  path.join(GLFW_SRC_DIR, "osmesa_context.c"),
  path.join(GLFW_SRC_DIR, "vulkan.c"),
  path.join(GLFW_SRC_DIR, "wgl_context.c"),
  path.join(GLFW_SRC_DIR, "win32_init.c"),
  path.join(GLFW_SRC_DIR, "win32_joystick.c"),
  path.join(GLFW_SRC_DIR, "win32_monitor.c"),
  path.join(GLFW_SRC_DIR, "win32_thread.c"),
  path.join(GLFW_SRC_DIR, "win32_time.c"),
  path.join(GLFW_SRC_DIR, "win32_window.c"),
  path.join(GLFW_SRC_DIR, "window.c"),
  path.join(GLFW_SRC_DIR, "**.h"),
  path.join(GLFW_INCLUDE_DIR, "GLFW", "**.h")
}

removefiles {
  path.join(GLFW_SRC_DIR, "**.bin.h")
}

includedirs {
  GLFW_SRC_DIR,
  path.join(GLFW_INCLUDE_DIR, "GLFW")
}

configuration {}
