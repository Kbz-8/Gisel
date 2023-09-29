set_languages("cxx17")

set_objectdir("build-int/$(os)_$(arch)")
set_targetdir("build/$(os)_$(arch)")

target("gisel")
    set_kind("static")
    add_files("src/*.cpp")
    add_includedirs("API", "src")
target_end()

target("giseli")
    set_default(false)
    set_kind("binary")
    add_deps("gisel")
    add_files("Interpreter/gisel.cpp")
    add_includedirs("API", "src")
target_end()
