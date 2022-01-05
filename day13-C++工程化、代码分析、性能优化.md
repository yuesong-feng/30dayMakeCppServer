# day13-C++工程化、代码分析、性能优化

在之前的教程里，我们已经完整开发了一个主从Reactor多线程的服务器的核心架构，接下来的开发重心应该从架构转移到细节。在这之前，将整个项目现代化、工程化是必要的，也是必须的。

C++项目工程化的第一步，一定是使用CMake。目前将所有文件都放在一个文件夹，并且没有分类。随着项目越来越复杂、模块越来越多，开发者需要考虑这座屎山的可读性，如将模块拆分到不同文件夹，将头文件统一放在一起等。对于这样复杂的项目，如果手写复杂的Makefile来编译链接，那么将会相当负责繁琐。我们应当使用CMake来管理我们的项目，CMake的使用非常简单、功能强大，会帮我们自动生成Makefile文件，使项目的编译链接更加容易，程序员可以将更多的精力放在写代码上。
> C++的编译、链接看似简单，实际上相当繁琐复杂，具体原理请参考《深入理解计算机系统（第三版）》第七章。如果没有CMake，开发一个大型C++项目，一半的时间会用在编译链接上。

我们将核心库放在`src`目录下，使用网络库的测试程序放在`test`目录下，所有的头文件放在`/include`目录下：
```
set(PINE_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(PINE_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
include_directories(${PINE_SRC_INCLUDE_DIR} ${PINE_TEST_INCLUDE_DIR})
```
实现头文件的`.cpp`文件则按照模块放在`src`目录（这个版本还未拆分模块到不同文件夹）。

`src`目录是网络库，并没有可执行的程序，我们只需要将这个网络库的`.cpp`文件编译链接成多个目标文件，然后链接到一个共享库中：
```
file(GLOB_RECURSE pine_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(pine_shared SHARED ${pine_sources})
```
在编译时，根据不同环境设置编译参数也很方便：
```
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++17 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") #TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
```
使用`test`目录下的`.cpp`文件创建可执行文件的代码：
```
foreach (pine_test_source ${PINE_TEST_SOURCES})
    get_filename_component(pine_test_filename ${pine_test_source} NAME)
    string(REPLACE ".cpp" "" pine_test_name ${pine_test_filename})

    add_executable(${pine_test_name} EXCLUDE_FROM_ALL ${pine_test_source})
    add_dependencies(build-tests ${pine_test_name})
    add_dependencies(check-tests ${pine_test_name})

    target_link_libraries(${pine_test_name} pine_shared)

    set_target_properties(${pine_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        COMMAND ${pine_test_name}
    )
endforeach(pine_test_source ${PINE_TEST_SOURCES})
```
注意我们切换到了更强大更好用的clang编译器（之前是GCC）。

配置好CMake和clang后，还需要做以下三件事：
1. format：作为一个大型C++项目，可能有许多程序员共同开发，每个人的编码习惯风格都不同，整个项目可能风格杂乱，可读性差，不利于项目维护。所以在写C++代码时应该遵守一些约定，使代码的风格统一。目前比较流行的C++代码风格有google、llvm等，本项目采用google风格。
2. cpplint：基于google C++编码规范的静态代码分析工具，可以查找代码中错误、违反约定、建议修改的地方。
3. clang-tidy：clang编译器的代码分析工具，功能十分强大。既可以查找代码中的各种静态错误，还可以提示可能会在运行时发生的问题。不仅如此，还可以通过代码分析给出可以提升程序性能的建议。

这三件事可以保证我们写出风格一致、bug较少、性能较好、遵守google编码规范的项目，是开发大型C++项目必备的利器。

为了很方便地自动一键运行，这三个工具都已经以`python`脚本的格式保存在了`build_support`目录：
```
build_support
    - clang_format_exclusions.txt     // 不需要格式化的代码
    - run_clang_format.py             // format
    - cpplint.py                      // cpplint
    - run_clang_tidy_extra.py         // 帮助文件，不直接运行
    - run_clang_tidy.py               // clang-tidy
.clang-format                         // format配置
.clang-tidy                           // clang-tidy配置
```

format在CMakeLists.txt中的配置：
```
# runs clang format and updates files in place.
add_custom_target(format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${PINE_FORMAT_DIRS}
        --fix
        --quiet
        )
```
cpplint在CMakeLists.txt中的配置：
```
add_custom_target(cpplint echo '${PINE_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting
        )
```
clang-tidy在CMakeLists.txt中的配置：    
```
add_custom_target(clang-tidy
        ${PINE_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}        # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                      # using cmake's generated compile commands
        )
```
这里省略了文件夹定义等很多信息，完整配置在源代码中。

接下来尝试编译我们的项目，首先创建一个`build`文件夹，防止文件和项目混在一起：
```
mkdir build
cd build
```
然后使用CMake生成Makefile：
```
cmake ..
```
生成Makefile后，使用以下命令进行代码格式化:
```
make format
```
然后用cpplint检查代码:
```
make cpplint
```
最后使用clang-tidy进行代码分析：
```
make clang-tidy
```
将所有的警告都修改好，重新运行这三个命令直到全部通过。然后使用`make`指令即可编译整个网络库，会被保存到`lib`文件夹中，但这里没有可执行文件。如果我们需要编译可执行服务器，需要编译`test`目录下相应的源文件:
```
make server
make multiple_client
make single_client
```
生成的可执行文件在`build/test`目录下，这时使用`./test/server`即可运行服务器。

至此，今天的教程已经结束了。今天我们将整个项目工程化，使用了CMake、format、cpplint、clang-tidy，代码的风格变成了google-style，修复了之前版本的许多bug，应用了这些工具给我们提供的现代C++项目建议，性能也提高了。在今天的版本，所有的类也都被声明为不可拷贝、不可移动。clang-tidy提示的按值传参也被修改为引用传参，减少了大量的复制操作。这些工具建议的修改都大大降低了bug发生的几率、提高了服务器性能，虽然还没有用任何的性能测试工具，服务器的处理速度、吞吐量、并发支持度都明显提高了。

完整源代码：[https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day13](https://github.com/yuesong-feng/30dayMakeCppServer/tree/main/code/day13)