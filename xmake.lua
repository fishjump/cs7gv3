set_languages("c17", "cxx17")
set_warnings("all", "error")

add_includedirs("module/cs7gvx_utils/include", "/opt/homebrew/Cellar/glm/0.9.9.8/include",
    "/opt/homebrew/Cellar/glfw/3.3.6/include", "/opt/homebrew/Cellar/boost/1.76.0/include",
    "/opt/homebrew/Cellar/assimp/5.2.0/include", "/opt/homebrew/Cellar/freetype/2.11.1/include/freetype2")

add_linkdirs("/opt/homebrew/Cellar/glm/0.9.9.8/lib", "/opt/homebrew/Cellar/glfw/3.3.6/lib",
    "/opt/homebrew/Cellar/boost/1.76.0/lib", "/opt/homebrew/Cellar/assimp/5.2.0/lib",
    "/opt/homebrew/Cellar/freetype/2.11.1/lib")

add_frameworks("OpenGL", "Cocoa")

includes("module/cs7gvx_utils")

target("cs7gv3-assignment1")
set_kind("binary")
add_includedirs("assignment1/include")
add_files("assignment1/src/**.cpp")
add_deps("cs7gvx-utils")
add_links("cs7gvx-utils", "boost_program_options", "boost_filesystem")

-- target("cs7gv3-assignment2")
-- set_kind("binary")
-- add_includedirs("include")
-- add_files("src/assignment2/**.cpp")
-- add_deps("cs7gvx-utils")
-- add_links("cs7gvx-utils", "boost_program_options", "boost_filesystem")
