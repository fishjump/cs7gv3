set_languages("c17", "cxx17")
-- set_warnings("all", "error")

add_includedirs("module/figine/include", "/opt/homebrew/include")

add_linkdirs("/opt/homebrew/lib")

add_frameworks("OpenGL", "Cocoa")

includes("module/figine")

target("assignment1")
    set_kind("binary")
    add_deps("figine")
    add_files("assignment1/**.cpp")
    add_links("figine")

target("assignment2")
    set_kind("binary")
    add_deps("figine")
    add_files("assignment2/**.cpp")
    add_links("figine")

target("assignment3")
    set_kind("binary")
    add_deps("figine")
    add_files("assignment3/**.cpp")
    add_links("figine")

target("assignment4")
    set_kind("binary")
    add_deps("figine")
    add_files("assignment4/**.cpp")
    add_links("figine")

target("assignment5")
    set_kind("binary")
    add_deps("figine")
    add_files("assignment5/**.cpp")
    add_links("figine")
