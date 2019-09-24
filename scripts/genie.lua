--
-- Copyright 2010-2019 Branimir Karadzic. All rights reserved.
-- License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
--

BVR_DIR = (path.getabsolute("..") .. "/")
BVR_SCRIPTS_DIR = path.join(BVR_DIR, "scripts")
EXTERNAL_DIR = (BVR_DIR .. "external/")

local BUILD_DIR = path.join(BVR_DIR, ".build")

--
-- Solution
--
solution "bvr"
language "C++"
configurations {"Debug", "Release"}
platforms {"x64"}
startproject "bvr"
location (BUILD_DIR)

targetdir (path.join(BUILD_DIR, "win64_" .. _ACTION, "bin", _name))
objdir (path.join(BUILD_DIR, "win64_" .. _ACTION, "obj", _name))

group "libs"
dofile(path.join(BVR_SCRIPTS_DIR, "bvr.lua"))
