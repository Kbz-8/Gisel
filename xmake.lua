set_languages("cxx17")

set_objectdir("build-int/$(os)_$(arch)")
set_targetdir("build/$(os)_$(arch)")

target("nir")
    set_kind("binary")
    add_files("src/*.cpp")
target_end() -- optional but I think the code is cleaner with this
