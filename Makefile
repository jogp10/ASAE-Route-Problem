# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /home/ze/Programming/ia_t03g34_assignment1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ze/Programming/ia_t03g34_assignment1

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ze/Programming/ia_t03g34_assignment1/CMakeFiles /home/ze/Programming/ia_t03g34_assignment1//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ze/Programming/ia_t03g34_assignment1/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named ASAEinspectionRoute

# Build rule for target.
ASAEinspectionRoute: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 ASAEinspectionRoute
.PHONY : ASAEinspectionRoute

# fast build rule for target.
ASAEinspectionRoute/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/build
.PHONY : ASAEinspectionRoute/fast

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/main.cpp.s
.PHONY : main.cpp.s

src/ASAE.o: src/ASAE.cpp.o
.PHONY : src/ASAE.o

# target to build an object file
src/ASAE.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/ASAE.cpp.o
.PHONY : src/ASAE.cpp.o

src/ASAE.i: src/ASAE.cpp.i
.PHONY : src/ASAE.i

# target to preprocess a source file
src/ASAE.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/ASAE.cpp.i
.PHONY : src/ASAE.cpp.i

src/ASAE.s: src/ASAE.cpp.s
.PHONY : src/ASAE.s

# target to generate assembly for a file
src/ASAE.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/ASAE.cpp.s
.PHONY : src/ASAE.cpp.s

src/Menu.o: src/Menu.cpp.o
.PHONY : src/Menu.o

# target to build an object file
src/Menu.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/Menu.cpp.o
.PHONY : src/Menu.cpp.o

src/Menu.i: src/Menu.cpp.i
.PHONY : src/Menu.i

# target to preprocess a source file
src/Menu.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/Menu.cpp.i
.PHONY : src/Menu.cpp.i

src/Menu.s: src/Menu.cpp.s
.PHONY : src/Menu.s

# target to generate assembly for a file
src/Menu.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/Menu.cpp.s
.PHONY : src/Menu.cpp.s

src/graph/graph.o: src/graph/graph.cpp.o
.PHONY : src/graph/graph.o

# target to build an object file
src/graph/graph.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/graph/graph.cpp.o
.PHONY : src/graph/graph.cpp.o

src/graph/graph.i: src/graph/graph.cpp.i
.PHONY : src/graph/graph.i

# target to preprocess a source file
src/graph/graph.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/graph/graph.cpp.i
.PHONY : src/graph/graph.cpp.i

src/graph/graph.s: src/graph/graph.cpp.s
.PHONY : src/graph/graph.s

# target to generate assembly for a file
src/graph/graph.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/ASAEinspectionRoute.dir/build.make CMakeFiles/ASAEinspectionRoute.dir/src/graph/graph.cpp.s
.PHONY : src/graph/graph.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... ASAEinspectionRoute"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
	@echo "... src/ASAE.o"
	@echo "... src/ASAE.i"
	@echo "... src/ASAE.s"
	@echo "... src/Menu.o"
	@echo "... src/Menu.i"
	@echo "... src/Menu.s"
	@echo "... src/graph/graph.o"
	@echo "... src/graph/graph.i"
	@echo "... src/graph/graph.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

