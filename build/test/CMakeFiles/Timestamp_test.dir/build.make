# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhuyi/桌面/WorkSpace/fst-muduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhuyi/桌面/WorkSpace/fst-muduo/build

# Include any dependencies generated for this target.
include test/CMakeFiles/Timestamp_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include test/CMakeFiles/Timestamp_test.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/Timestamp_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/Timestamp_test.dir/flags.make

test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o: test/CMakeFiles/Timestamp_test.dir/flags.make
test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o: ../test/Timestamp_test.cc
test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o: test/CMakeFiles/Timestamp_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhuyi/桌面/WorkSpace/fst-muduo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o"
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test && clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o -MF CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o.d -o CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o -c /home/zhuyi/桌面/WorkSpace/fst-muduo/test/Timestamp_test.cc

test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.i"
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test && clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zhuyi/桌面/WorkSpace/fst-muduo/test/Timestamp_test.cc > CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.i

test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.s"
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test && clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zhuyi/桌面/WorkSpace/fst-muduo/test/Timestamp_test.cc -o CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.s

# Object files for target Timestamp_test
Timestamp_test_OBJECTS = \
"CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o"

# External object files for target Timestamp_test
Timestamp_test_EXTERNAL_OBJECTS =

../bin/Timestamp_test: test/CMakeFiles/Timestamp_test.dir/Timestamp_test.cc.o
../bin/Timestamp_test: test/CMakeFiles/Timestamp_test.dir/build.make
../bin/Timestamp_test: libfst_muduo.so
../bin/Timestamp_test: test/CMakeFiles/Timestamp_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zhuyi/桌面/WorkSpace/fst-muduo/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/Timestamp_test"
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Timestamp_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/Timestamp_test.dir/build: ../bin/Timestamp_test
.PHONY : test/CMakeFiles/Timestamp_test.dir/build

test/CMakeFiles/Timestamp_test.dir/clean:
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test && $(CMAKE_COMMAND) -P CMakeFiles/Timestamp_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/Timestamp_test.dir/clean

test/CMakeFiles/Timestamp_test.dir/depend:
	cd /home/zhuyi/桌面/WorkSpace/fst-muduo/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhuyi/桌面/WorkSpace/fst-muduo /home/zhuyi/桌面/WorkSpace/fst-muduo/test /home/zhuyi/桌面/WorkSpace/fst-muduo/build /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test /home/zhuyi/桌面/WorkSpace/fst-muduo/build/test/CMakeFiles/Timestamp_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/Timestamp_test.dir/depend

