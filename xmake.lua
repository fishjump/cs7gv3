set_languages("c17", "cxx17")

add_includedirs("common/include")
add_files("common/src/**.cpp")

if is_plat("macosx") then
  add_includedirs("/opt/homebrew/Cellar/glew/2.2.0_1/include")
  add_linkdirs("/opt/homebrew/Cellar/glew/2.2.0_1/lib")

  add_includedirs("/opt/homebrew/Cellar/boost/1.76.0/include")
  add_linkdirs("/opt/homebrew/Cellar/boost/1.76.0/lib")

  add_includedirs("/opt/homebrew/Cellar/assimp/5.2.0/include")
  add_linkdirs("/opt/homebrew/Cellar/assimp/5.2.0/lib")

  add_links("GLEW")

  add_frameworks("OpenGL", "GLUT", "Cocoa")
end

target("cs7gv3-main")
  set_kind("binary")
  add_files("main/src/**.cpp")
  add_includedirs("main/include")
  add_links("boost_program_options", "boost_filesystem", "assimp")