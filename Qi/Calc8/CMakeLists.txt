# CMakeList.txt: Calc8 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
add_executable(Calc8
        main.cpp
        compiler.cpp
        vm.cpp
        annotation.hpp
        ast.hpp
        compiler.hpp
        error_handler.hpp
        expression.hpp
        statement.hpp
        vm.hpp
)

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif ()

find_package(Boost REQUIRED)
target_link_libraries(Calc8 PRIVATE Boost::headers)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Calc8 PROPERTY CXX_STANDARD 20)
endif ()

# TODO: 如有需要，请添加测试并安装目标。
