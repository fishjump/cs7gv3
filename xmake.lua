set_languages("c17", "cxx17")

add_includedirs("common/include")
add_files("common/src/**.cpp")

add_includedirs("/opt/homebrew/Cellar/glew/2.2.0_1/include")
add_linkdirs("/opt/homebrew/Cellar/glew/2.2.0_1/lib")

add_includedirs("/opt/homebrew/Cellar/boost/1.76.0/include")
add_linkdirs("/opt/homebrew/Cellar/boost/1.76.0/lib")

add_links("GLEW")

if is_plat("macosx") then
  add_frameworks("OpenGL", "GLUT", "Cocoa")
elseif is_plat("windows") then
else      
end

target("cs7gv3-shader-compile")
  set_kind("binary")
  add_files("shader/src/**.cpp")
  add_includedirs("shader/include")
  add_links("boost_program_options")
  add_links("boost_filesystem")

target("cs7gv3-main")
  set_kind("binary")
  add_files("main/src/**.cpp")
  add_includedirs("main/include")
 
