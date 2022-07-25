set_languages("cxx17")

set_objectdir("build-int/$(os)_$(arch)")
set_targetdir("build/$(os)_$(arch)")

target("nir")
    set_kind("static")
    add_files("src/*.cpp")
    add_includedirs("API", "src")
target_end()

target("nirc")
    set_default(false)
    set_kind("binary")
    add_deps("nir")
    add_files("Interpreter/nir.cpp")
    add_includedirs("API", "src")
target_end()
