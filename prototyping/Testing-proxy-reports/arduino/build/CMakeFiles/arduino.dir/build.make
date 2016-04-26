# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chap/OpenDaVINCI/automotive/miniature/arduino

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chap/OpenDaVINCI/automotive/miniature/arduino/build

# Include any dependencies generated for this target.
include CMakeFiles/arduino.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/arduino.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/arduino.dir/flags.make

CMakeFiles/arduino.dir/apps/arduino.cpp.o: CMakeFiles/arduino.dir/flags.make
CMakeFiles/arduino.dir/apps/arduino.cpp.o: ../apps/arduino.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduino.dir/apps/arduino.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduino.dir/apps/arduino.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/apps/arduino.cpp

CMakeFiles/arduino.dir/apps/arduino.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduino.dir/apps/arduino.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/apps/arduino.cpp > CMakeFiles/arduino.dir/apps/arduino.cpp.i

CMakeFiles/arduino.dir/apps/arduino.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduino.dir/apps/arduino.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/apps/arduino.cpp -o CMakeFiles/arduino.dir/apps/arduino.cpp.s

CMakeFiles/arduino.dir/apps/arduino.cpp.o.requires:
.PHONY : CMakeFiles/arduino.dir/apps/arduino.cpp.o.requires

CMakeFiles/arduino.dir/apps/arduino.cpp.o.provides: CMakeFiles/arduino.dir/apps/arduino.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduino.dir/build.make CMakeFiles/arduino.dir/apps/arduino.cpp.o.provides.build
.PHONY : CMakeFiles/arduino.dir/apps/arduino.cpp.o.provides

CMakeFiles/arduino.dir/apps/arduino.cpp.o.provides.build: CMakeFiles/arduino.dir/apps/arduino.cpp.o

# Object files for target arduino
arduino_OBJECTS = \
"CMakeFiles/arduino.dir/apps/arduino.cpp.o"

# External object files for target arduino
arduino_EXTERNAL_OBJECTS =

arduino: CMakeFiles/arduino.dir/apps/arduino.cpp.o
arduino: CMakeFiles/arduino.dir/build.make
arduino: libarduinolib-static.a
arduino: /usr/lib/libopendavinci.so
arduino: /usr/lib/x86_64-linux-gnu/librt.a
arduino: /usr/lib/libautomotivedata.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_core.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_flann.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_ml.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so
arduino: /usr/lib/x86_64-linux-gnu/libopencv_video.so
arduino: CMakeFiles/arduino.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable arduino"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arduino.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/arduino.dir/build: arduino
.PHONY : CMakeFiles/arduino.dir/build

CMakeFiles/arduino.dir/requires: CMakeFiles/arduino.dir/apps/arduino.cpp.o.requires
.PHONY : CMakeFiles/arduino.dir/requires

CMakeFiles/arduino.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/arduino.dir/cmake_clean.cmake
.PHONY : CMakeFiles/arduino.dir/clean

CMakeFiles/arduino.dir/depend:
	cd /home/chap/OpenDaVINCI/automotive/miniature/arduino/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chap/OpenDaVINCI/automotive/miniature/arduino /home/chap/OpenDaVINCI/automotive/miniature/arduino /home/chap/OpenDaVINCI/automotive/miniature/arduino/build /home/chap/OpenDaVINCI/automotive/miniature/arduino/build /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles/arduino.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/arduino.dir/depend

