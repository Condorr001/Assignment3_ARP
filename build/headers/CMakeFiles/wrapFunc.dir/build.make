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
CMAKE_SOURCE_DIR = /home/condor001/Documenti/GitHub/Assignment3_ARP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/condor001/Documenti/GitHub/Assignment3_ARP/build

# Include any dependencies generated for this target.
include headers/CMakeFiles/wrapFunc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include headers/CMakeFiles/wrapFunc.dir/compiler_depend.make

# Include the progress variables for this target.
include headers/CMakeFiles/wrapFunc.dir/progress.make

# Include the compile flags for this target's objects.
include headers/CMakeFiles/wrapFunc.dir/flags.make

headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o: headers/CMakeFiles/wrapFunc.dir/flags.make
headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o: ../headers/wrapFuncs/wrapFunc.c
headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o: headers/CMakeFiles/wrapFunc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/condor001/Documenti/GitHub/Assignment3_ARP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o"
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o -MF CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o.d -o CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o -c /home/condor001/Documenti/GitHub/Assignment3_ARP/headers/wrapFuncs/wrapFunc.c

headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.i"
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/condor001/Documenti/GitHub/Assignment3_ARP/headers/wrapFuncs/wrapFunc.c > CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.i

headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.s"
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/condor001/Documenti/GitHub/Assignment3_ARP/headers/wrapFuncs/wrapFunc.c -o CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.s

# Object files for target wrapFunc
wrapFunc_OBJECTS = \
"CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o"

# External object files for target wrapFunc
wrapFunc_EXTERNAL_OBJECTS =

headers/libwrapFunc.a: headers/CMakeFiles/wrapFunc.dir/wrapFuncs/wrapFunc.c.o
headers/libwrapFunc.a: headers/CMakeFiles/wrapFunc.dir/build.make
headers/libwrapFunc.a: headers/CMakeFiles/wrapFunc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/condor001/Documenti/GitHub/Assignment3_ARP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libwrapFunc.a"
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && $(CMAKE_COMMAND) -P CMakeFiles/wrapFunc.dir/cmake_clean_target.cmake
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wrapFunc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
headers/CMakeFiles/wrapFunc.dir/build: headers/libwrapFunc.a
.PHONY : headers/CMakeFiles/wrapFunc.dir/build

headers/CMakeFiles/wrapFunc.dir/clean:
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers && $(CMAKE_COMMAND) -P CMakeFiles/wrapFunc.dir/cmake_clean.cmake
.PHONY : headers/CMakeFiles/wrapFunc.dir/clean

headers/CMakeFiles/wrapFunc.dir/depend:
	cd /home/condor001/Documenti/GitHub/Assignment3_ARP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/condor001/Documenti/GitHub/Assignment3_ARP /home/condor001/Documenti/GitHub/Assignment3_ARP/headers /home/condor001/Documenti/GitHub/Assignment3_ARP/build /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers /home/condor001/Documenti/GitHub/Assignment3_ARP/build/headers/CMakeFiles/wrapFunc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : headers/CMakeFiles/wrapFunc.dir/depend
