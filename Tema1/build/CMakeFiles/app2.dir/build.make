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
CMAKE_SOURCE_DIR = /home/junior/Desktop/Tema1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/junior/Desktop/Tema1/build

# Include any dependencies generated for this target.
include CMakeFiles/app2.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/app2.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/app2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/app2.dir/flags.make

CMakeFiles/app2.dir/app2.c.o: CMakeFiles/app2.dir/flags.make
CMakeFiles/app2.dir/app2.c.o: ../app2.c
CMakeFiles/app2.dir/app2.c.o: CMakeFiles/app2.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/junior/Desktop/Tema1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/app2.dir/app2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/app2.dir/app2.c.o -MF CMakeFiles/app2.dir/app2.c.o.d -o CMakeFiles/app2.dir/app2.c.o -c /home/junior/Desktop/Tema1/app2.c

CMakeFiles/app2.dir/app2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/app2.dir/app2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/junior/Desktop/Tema1/app2.c > CMakeFiles/app2.dir/app2.c.i

CMakeFiles/app2.dir/app2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/app2.dir/app2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/junior/Desktop/Tema1/app2.c -o CMakeFiles/app2.dir/app2.c.s

# Object files for target app2
app2_OBJECTS = \
"CMakeFiles/app2.dir/app2.c.o"

# External object files for target app2
app2_EXTERNAL_OBJECTS =

app2: CMakeFiles/app2.dir/app2.c.o
app2: CMakeFiles/app2.dir/build.make
app2: CMakeFiles/app2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/junior/Desktop/Tema1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable app2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/app2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/app2.dir/build: app2
.PHONY : CMakeFiles/app2.dir/build

CMakeFiles/app2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/app2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/app2.dir/clean

CMakeFiles/app2.dir/depend:
	cd /home/junior/Desktop/Tema1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/junior/Desktop/Tema1 /home/junior/Desktop/Tema1 /home/junior/Desktop/Tema1/build /home/junior/Desktop/Tema1/build /home/junior/Desktop/Tema1/build/CMakeFiles/app2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/app2.dir/depend

