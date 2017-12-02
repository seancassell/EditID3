# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library:
cmake_minimum_required(VERSION 3.4.1)


# Project name:
project(libeditid3)


# Compile as C++:
enable_language(CXX)


# Require C++14:
set(CMAKE_CXX_STANDARD 14)


# Set project version:
set(VERSION_MAJOR 1)
set(VERSION_MINOR 12)
set(VERSION_PATCH 335)
set(VERSION_TWEAK 2)


# Search for a specified prebuilt library, and store the path as a 
# variable. Because CMake includes system libraries in the search path by 
# default, you only need to specify the name of the public NDK library you want
# add. CMake verifies that the library exists before completing its build.
find_library(	# Set the name of the PATH variable:
				log-lib
				
				# Specify the name of the NDK library that
				# you want CMake to locate:
				log
)



# Specify libraries CMake should link to your target library. You can link
# multiple libraries, such as libraries you define in this build script, 
# prebuilt third-party libraries, or system libraries:
target_link_libraries(	# Specifies the target library.
						libeditid3
						
						# Links the target library to the log library
						# included in the NDK.
						${log-lib}
)


# Create and name the library, set it as either STATIC or SHARED, and provide
# the relative paths to its source code. You can define multiple libraries, 
# and CMake builds them for you. Gradle automatically packages shared libraries
# with your APK:
add_library(# Set the name of the library:
			libeditid3
			
			# Set the library as a shared library:
			SHARED
			
			# Provides a relative path to your source file(s):
			src/main/cpp/id3v2lib/constants.hpp
			src/main/cpp/id3v2lib/frame.hpp
			src/main/cpp/id3v2lib/header.hpp
			src/main/cpp/id3v2lib/types.hpp
			src/main/cpp/id3v2lib/utils.hpp
			src/main/cpp/EditID3.cpp
			src/main/cpp/EditID3.h
			src/main/cpp/frame.cpp
			src/main/cpp/header.cpp
			src/main/cpp/id3v2lib.cpp
			src/main/cpp/id3v2lib.hpp
			src/main/cpp/pch.h
			src/main/cpp/types.cpp
			src/main/cpp/utils.cpp
)


# Specify compiler options:
#add_compile_options(-Wall -Wextra -Wpedantic)
