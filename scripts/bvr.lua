BVR_SRC_DIR = path.join(BVR_DIR, "src")
BVR_INCLUDE_DIR = path.join(BVR_DIR, "include")
EXTERNAL_INCLUDE_DIR = path.join(EXTERNAL_DIR, "include")
VK_DIR = os.getenv("VK_SDK_PATH")

project("BVR")
uuid(os.uuid("BVR"))
kind "ConsoleApp"

flags {
  "FatalWarnings"
}

defines {
  "_HAS_ITERATOR_DEBUGGING=0",
  "_SECURE_SCL=0",
  "WIN32_LEAN_AND_MEAN"
}

files {
  path.join(BVR_SRC_DIR, "**.cpp"),
  path.join(BVR_INCLUDE_DIR, "**.h")
  -- path.join(MIKKTSPACE_DIR, "mikktspace.c")
}

removefiles {
  path.join(BVR_DIR, "**.bin.h")
}

includedirs {
  BVR_INCLUDE_DIR,
  EXTERNAL_INCLUDE_DIR,
  path.join(VK_DIR, "Include")
}

links {
  path.join(EXTERNAL_DIR, "libs", "glfw3"),
  path.join(VK_DIR, "Lib", "vulkan-1")
}

configuration "Debug"
  flags { "Symbols" }

configuration "Release"
  flags { "OptimizeSpeed" }

configuration {}
