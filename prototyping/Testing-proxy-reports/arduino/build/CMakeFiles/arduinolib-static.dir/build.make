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
include CMakeFiles/arduinolib-static.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/arduinolib-static.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/arduinolib-static.dir/flags.make

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o: CMakeFiles/arduinolib-static.dir/flags.make
CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o: ../src/Arduino.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Arduino.cpp

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Arduino.cpp > CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.i

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Arduino.cpp -o CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.s

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.requires:
.PHONY : CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.requires

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.provides: CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduinolib-static.dir/build.make CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.provides.build
.PHONY : CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.provides

CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.provides.build: CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o: CMakeFiles/arduinolib-static.dir/flags.make
CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o: ../src/CustomNetstringsProtocol.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/CustomNetstringsProtocol.cpp

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/CustomNetstringsProtocol.cpp > CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.i

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/CustomNetstringsProtocol.cpp -o CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.s

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.requires:
.PHONY : CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.requires

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.provides: CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduinolib-static.dir/build.make CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.provides.build
.PHONY : CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.provides

CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.provides.build: CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o: CMakeFiles/arduinolib-static.dir/flags.make
CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o: ../src/Camera.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Camera.cpp

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduinolib-static.dir/src/Camera.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Camera.cpp > CMakeFiles/arduinolib-static.dir/src/Camera.cpp.i

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduinolib-static.dir/src/Camera.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/Camera.cpp -o CMakeFiles/arduinolib-static.dir/src/Camera.cpp.s

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.requires:
.PHONY : CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.requires

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.provides: CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduinolib-static.dir/build.make CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.provides.build
.PHONY : CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.provides

CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.provides.build: CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o: CMakeFiles/arduinolib-static.dir/flags.make
CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o: ../src/OpenCVCamera.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/OpenCVCamera.cpp

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/OpenCVCamera.cpp > CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.i

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/OpenCVCamera.cpp -o CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.s

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.requires:
.PHONY : CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.requires

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.provides: CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduinolib-static.dir/build.make CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.provides.build
.PHONY : CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.provides

CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.provides.build: CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o: CMakeFiles/arduinolib-static.dir/flags.make
CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o: ../src/uEyeCamera.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o -c /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/uEyeCamera.cpp

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/uEyeCamera.cpp > CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.i

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/chap/OpenDaVINCI/automotive/miniature/arduino/src/uEyeCamera.cpp -o CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.s

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.requires:
.PHONY : CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.requires

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.provides: CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.requires
	$(MAKE) -f CMakeFiles/arduinolib-static.dir/build.make CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.provides.build
.PHONY : CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.provides

CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.provides.build: CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o

# Object files for target arduinolib-static
arduinolib__static_OBJECTS = \
"CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o" \
"CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o" \
"CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o" \
"CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o" \
"CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o"

# External object files for target arduinolib-static
arduinolib__static_EXTERNAL_OBJECTS =

libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/build.make
libarduinolib-static.a: CMakeFiles/arduinolib-static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libarduinolib-static.a"
	$(CMAKE_COMMAND) -P CMakeFiles/arduinolib-static.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arduinolib-static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/arduinolib-static.dir/build: libarduinolib-static.a
.PHONY : CMakeFiles/arduinolib-static.dir/build

CMakeFiles/arduinolib-static.dir/requires: CMakeFiles/arduinolib-static.dir/src/Arduino.cpp.o.requires
CMakeFiles/arduinolib-static.dir/requires: CMakeFiles/arduinolib-static.dir/src/CustomNetstringsProtocol.cpp.o.requires
CMakeFiles/arduinolib-static.dir/requires: CMakeFiles/arduinolib-static.dir/src/Camera.cpp.o.requires
CMakeFiles/arduinolib-static.dir/requires: CMakeFiles/arduinolib-static.dir/src/OpenCVCamera.cpp.o.requires
CMakeFiles/arduinolib-static.dir/requires: CMakeFiles/arduinolib-static.dir/src/uEyeCamera.cpp.o.requires
.PHONY : CMakeFiles/arduinolib-static.dir/requires

CMakeFiles/arduinolib-static.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/arduinolib-static.dir/cmake_clean.cmake
.PHONY : CMakeFiles/arduinolib-static.dir/clean

CMakeFiles/arduinolib-static.dir/depend:
	cd /home/chap/OpenDaVINCI/automotive/miniature/arduino/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chap/OpenDaVINCI/automotive/miniature/arduino /home/chap/OpenDaVINCI/automotive/miniature/arduino /home/chap/OpenDaVINCI/automotive/miniature/arduino/build /home/chap/OpenDaVINCI/automotive/miniature/arduino/build /home/chap/OpenDaVINCI/automotive/miniature/arduino/build/CMakeFiles/arduinolib-static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/arduinolib-static.dir/depend

