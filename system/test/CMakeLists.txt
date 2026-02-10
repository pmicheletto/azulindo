# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(interfaceTests LANGUAGES CXX)

# ---- Tests ----

add_executable(interface_test source/interface_test.cpp)
target_link_libraries(interface_test PRIVATE interface_lib)
target_compile_features(interface_test PRIVATE cxx_std_17)

add_test(NAME interface_test COMMAND interface_test)

# ---- End-of-file commands ----

add_folders(Test)
