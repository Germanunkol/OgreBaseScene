cmake_minimum_required(VERSION 2.6)

project(OgreBaseScene)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

if(WIN32)
	# On win, require user to give paths to Oculus SDK and Ogre:

	# Find Ogre files:
	if (NOT DEFINED ENV{OGRE_HOME} AND NOT DEFINED OGRE_HOME)
		message(SEND_ERROR "Must define OGRE_HOME.")
	else()
		if (NOT DEFINED OGRE_HOME)
			set(OGRE_HOME ENV{OGRE_HOME})
		endif()
		if (NOT DEFINED OGRE_BUILD OR "${OGRE_BUILD}" STREQUAL "")
			set(OGRE_BUILD ${OGRE_HOME})
		endif()
		if( NOT DEFINED OGRE_SOURCE_DIR OR "${OGRE_SOURCE_DIR}" STREQUAL "")
			set(OGRE_SOURCE_DIR ${OGRE_HOME})
		endif()
	endif()
	
	set(OIS_AND_OGRE_LIB_DIRS
		${OGRE_HOME}/lib/debug
		${OGRE_HOME}/lib/Release
		${OGRE_HOME}/bin/debug
		${OGRE_HOME}/bin/Release
	)
	
else()

endif()

# Get the Packages (finds the library and its include files):
find_package(OGRE REQUIRED)

find_package(OIS REQUIRED)
find_package(ITK REQUIRED)
#include( ${ITK_USE_FILE} )

find_package(Boost COMPONENTS system filesystem regex REQUIRED)

include_directories(
	${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_Overlay_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	${GDCM_INCLUDE_DIR}
	#${FREETYPE_INCLUDE_DIRS}
	#${FREETYPE_INCLUDE_DIR_freetype2}
	include
	src
	#MyGUI/MyGUIEngine/include
	#MyGUI/Platforms/Ogre/OgrePlatform/include
	#MyGUI/Common/
	#MyGUI/UnitTests/UnitTest_RTTLayer/
	#MyGUI/UnitTests/UnitTest_HyperTextBox/
)

link_directories(
	#${XINERAMA_LINK_DIR}
	${GDCM_INCLUDE_DIR}
	#${PROJECT_BINARY_DIR}/MyGUI/lib/
	${OIS_AND_OGRE_LIB_DIRS}
	${Boost_LIBRARY_DIRS}
)

set(HDRS
	./include/SimpleFrameListener.h
)
 
set(SRCS
	./main.cpp
	./src/SimpleFrameListener.cpp
)

add_executable(OgreBaseScene ${HDRS} ${SRCS})
target_link_libraries( OgreBaseScene
	${OGRE_LIBRARIES}
	${OGRE_Overlay_LIBRARIES}
	${OIS_LIBRARIES}
	#${ITK_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
)

if( UNIX )
	target_link_libraries(OgreBaseScene
		#debug MyGUIEngine_d.so
		#ebug MyGUI.OgrePlatform_d.so
		#debug OIS_d.so
		#ptimized MyGUIEngine.so
		#ptimized MyGUI.OgrePlatform.so
		optimized OIS.so
	)
elseif( WIN32 )
	target_link_libraries(OgreBaseScene
		# Others:
		#ebug MyGUIEngine_d.dll
		#ebug MyGUI.OgrePlatform_d.dll
		debug OIS_d.dll
		#ptimized MyGUIEngine.dll
		#ptimized MyGUI.OgrePlatform.dll
		optimized OIS.dll
	)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# Copy files needed by Ogre3D (plugins.cfg and resources.cfg)
message(STATUS "Ogre plugin dir found: ${OGRE_PLUGIN_DIR_REL}")
file(COPY plugins.cfg DESTINATION . )
file(COPY plugins_d.cfg DESTINATION . )
if (OGRE_RenderSystem_GL_FOUND)
	# Change path to Ogre in plugins.cfg:
	file(READ ${PROJECT_BINARY_DIR}/plugins.cfg PLUGINS_FILE)
	file(READ ${PROJECT_BINARY_DIR}/plugins_d.cfg PLUGINS_D_FILE)

	if( WIN32 )
		string(REGEX REPLACE "PluginFolder=[^\n]*"
			"PluginFolder=." PLUGINS_FILE_MODIFIED
			${PLUGINS_FILE})
		string(REGEX REPLACE "PluginFolder=[^\n]*"
			"PluginFolder=." PLUGINS_D_FILE_MODIFIED
			${PLUGINS_D_FILE})
	else()
		string(REGEX REPLACE "PluginFolder=[^\n]*"
			"PluginFolder=${OGRE_PLUGIN_DIR_REL}" PLUGINS_FILE_MODIFIED
			${PLUGINS_FILE})
		string(REGEX REPLACE "PluginFolder=[^\n]*"
			"PluginFolder=${OGRE_PLUGIN_DIR_DBG}" PLUGINS_D_FILE_MODIFIED
			${PLUGINS_D_FILE})
	endif()
	file(WRITE ${PROJECT_BINARY_DIR}/plugins.cfg ${PLUGINS_FILE_MODIFIED})
	file(WRITE ${PROJECT_BINARY_DIR}/plugins_d.cfg ${PLUGINS_D_FILE_MODIFIED})
endif()

file(COPY resources.cfg DESTINATION . )

file(COPY media DESTINATION . )

# Copy Licenses:
#file(COPY Licenses/ DESTINATION Licenses/ )
#file(COPY LICENSE.txt DESTINATION . )
file(COPY README.md DESTINATION . )

message(STATUS ${Boost_SYSTEM_LIBRARY_DEBUG})

# Copy DLLs etc:
if( WIN32 )
	# /MP for multiple thread compiling
	# WIN32_LEAN_AND_MEAN to avoid conflicts between boost and Leap motion, which both seem to include winsock.h
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DWIN32_LEAN_AND_MEAN")
	
	# Copy the Ogre DLLs:
	file(COPY ${OGRE_HOME}/bin/debug/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "*.dll")
	file(COPY ${OGRE_HOME}/bin/Release/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "*.dll")
	file(COPY ${OGRE_HOME}/bin/Release/ DESTINATION ${PROJECT_BINARY_DIR}/Release FILES_MATCHING PATTERN "*.dll")

	if(Boost_FOUND)

		################################
		# Copy the Boost libraries:
		
		#System:
		get_filename_component( DIR ${Boost_SYSTEM_LIBRARY_DEBUG} DIRECTORY )
		get_filename_component( S ${Boost_SYSTEM_LIBRARY_DEBUG} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_DEBUG}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path               # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path

		get_filename_component( DIR ${Boost_SYSTEM_LIBRARY_RELEASE} DIRECTORY )
		get_filename_component( S ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_RELEASE}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
				
			#Filesystem:
		get_filename_component( DIR ${Boost_FILESYSTEM_LIBRARY_DEBUG} DIRECTORY )
		get_filename_component( S ${Boost_FILESYSTEM_LIBRARY_DEBUG} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_DEBUG}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path               # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path

		get_filename_component( DIR ${Boost_FILESYSTEM_LIBRARY_RELEASE} DIRECTORY )
		get_filename_component( S ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_RELEASE}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
		
			#REGEX:
		get_filename_component( DIR ${Boost_REGEX_LIBRARY_DEBUG} DIRECTORY )
		get_filename_component( S ${Boost_REGEX_LIBRARY_DEBUG} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_DEBUG}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path               # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path

		get_filename_component( DIR ${Boost_REGEX_LIBRARY_RELEASE} DIRECTORY )
		get_filename_component( S ${Boost_REGEX_LIBRARY_RELEASE} NAME_WE )

		add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${Boost_SYSTEM_LIBRARY_RELEASE}"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
				
			add_custom_command(TARGET OgreBaseScene POST_BUILD        # Adds a post-build event to MyTest
			COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
				"${DIR}/${S}.dll"      # <--this is in-file
				$<TARGET_FILE_DIR:OgreBaseScene>)                 # <--this is out-file path
	
	endif()
	
	# Find and copy freetype:
	#	find_package(Freetype)
	#file(COPY ${FREETYPE_LIBRARIES} DESTINATION Debug/ )
	#file(COPY ${FREETYPE_LIBRARIES} DESTINATION Release/ )
	
	#get_filename_component(FREETYPE_DIR ${FREETYPE_LIBRARIES} DIRECTORY)
	#get_filename_component(FREETYPE_DIR ${FREETYPE_DIR} DIRECTORY)
	#message(STATUS FREETYPE_LIBRARIES)
	#message(STATUS ${FREETYPE_LIBRARIES} ${FREETYPE_DIR})
	#file(COPY ${FREETYPE_DIR}/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Release FILES_MATCHING PATTERN "freetype*.dll")
	#file(COPY ${FREETYPE_DIR}/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Release FILES_MATCHING PATTERN "zlib*.dll")
	#file(COPY ${FREETYPE_DIR}/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "freetype*.dll")
	#file(COPY ${FREETYPE_DIR}/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "zlib*.dll")

endif()
