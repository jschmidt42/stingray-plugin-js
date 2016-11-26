cmake_minimum_required(VERSION 3.4)

# This module is shared; use include blocker.
if( _SETTINGS_ )
	return()
endif()
set(_SETTINGS_ 1)

include(CMakeMacros)