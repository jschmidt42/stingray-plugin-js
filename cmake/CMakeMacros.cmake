cmake_minimum_required(VERSION 3.4)

# Set each source file proper source group
macro(set_source_groups pList)
	foreach(FilePath ${pList})
		get_filename_component(DirName ${FilePath} DIRECTORY)
		if( NOT "${DirName}" STREQUAL "" )
			string(REGEX REPLACE "[.][.][/]" "" GroupName "${DirName}")
			if( MSVC )
				string(REGEX REPLACE "/" "\\\\" GroupName "${GroupName}")
			endif()
			source_group("${GroupName}" FILES ${FilePath})
		else()
			source_group("" FILES ${FilePath})
		endif()
	endforeach()
endmacro()

# Get all source files recursively and add them to pResult
macro(find_source_files pResult)
	set(FileList)
	set(SearchDir "${ARGN}")

	# Retrive all source files recursively
	set(FileExtensions)
	list(APPEND FileExtensions "*.h" "*.c" "*.cpp" "*.inl")
	if( PLATFORM_OSX OR PLATFORM_IOS )
		list(APPEND FileExtensions "*.m" "*.mm")
	endif()
	if( "${SearchDir}" STREQUAL "" )
		file(GLOB_RECURSE FileList RELATIVE ${PROJECT_SOURCE_DIR} ${FileExtensions})
	else()
		set(UpdatedFileExtensions)
		foreach(FileExtension ${FileExtensions})
			list(APPEND UpdatedFileExtensions "${SearchDir}/${FileExtension}")
		endforeach()
		file(GLOB_RECURSE FileList RELATIVE ${PROJECT_SOURCE_DIR} ${UpdatedFileExtensions})
	endif()
	list(APPEND ${pResult} ${FileList})

	set_source_groups("${FileList}")

	# Patch for Android compiler that refuse -std=c++11 flag on .c files.
	# Normally we would use CMAKE_CXX_FLAGS to add this flag only to .cpp files,
	# but somehow with NVidia NSight Tegra it also passes to .c files.
	if( PLATFORM_ANDROID OR PLATFORM_WEBGL )
		foreach(FilePath ${FileList})
			get_filename_component(ExtName ${FilePath} EXT)
			if( "${ExtName}" STREQUAL ".cpp" )
				set_source_files_properties(${FilePath} PROPERTIES COMPILE_FLAGS "-std=c++11")
			endif()
		endforeach()
	endif()
endmacro()