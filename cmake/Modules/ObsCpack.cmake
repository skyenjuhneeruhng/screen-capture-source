
if(APPLE AND NOT CPACK_GENERATOR)
	set(CPACK_GENERATOR "Bundle")
elseif(WIN32 AND NOT CPACK_GENERATOR)
	set(CPACK_GENERATOR "WIX" "ZIP")
endif()

set(CPACK_PACKAGE_NAME "Screen Recorder")
set(CPACK_PACKAGE_VENDOR "yourwebsite.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Screen Recorder - Live screen recording application.")

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

if(NOT DEFINED OBS_VERSION_OVERRIDE)
	if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
		execute_process(COMMAND git describe --always --tags --dirty=-modified
			OUTPUT_VARIABLE OBS_VERSION
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	else()
		set(OBS_VERSION "${CPACK_PACKAGE_VERSION}")
	endif()
else()
	set(OBS_VERSION "${OBS_VERSION_OVERRIDE}")
endif()

MESSAGE(STATUS "OBS_VERSION: ${OBS_VERSION}")

if(INSTALLER_RUN)
	set(CPACK_PACKAGE_EXECUTABLES
		"scap32" "Screen Recorder (32bit)"
		"scap64" "Screen Recorder (64bit)")
	set(CPACK_CREATE_DESKTOP_LINKS
		"scap32"
		"scap64")
else()
	if(WIN32)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(_output_suffix "64")
		else()
			set(_output_suffix "32")
		endif()
	else()
		set(_output_suffix "")
	endif()

	set(CPACK_PACKAGE_EXECUTABLES "scap${_output_suffix}" "Screen Recorder")
	set(CPACK_CREATE_DESKTOP_LINKS "scap${_output_suffix}")
endif()

set(CPACK_BUNDLE_NAME "OBS")
set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/cmake/osxbundle/Info.plist")
set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/cmake/osxbundle/obs.icns")
set(CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_SOURCE_DIR}/cmake/osxbundle/obslaunch.sh")

set(CPACK_WIX_TEMPLATE "${CMAKE_SOURCE_DIR}/cmake/Modules/WIX.template.in")

if(INSTALLER_RUN)
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "SCAP64")
	set(CPACK_WIX_UPGRADE_GUID "1f59ff79-2a3c-43c1-b2b2-033a5e6342eb")
	set(CPACK_WIX_PRODUCT_GUID "0c7bec2a-4f07-41b2-9dff-d64b09c9c384")
	set(CPACK_PACKAGE_FILE_NAME "scap-x86-${OBS_VERSION}")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	if(WIN32)
		set(CPACK_PACKAGE_NAME "scap-x64")
	endif()
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "SCAP64")
	set(CPACK_WIX_UPGRADE_GUID "44c72510-2e8e-489c-8bc0-2011a9631b0b")
	set(CPACK_WIX_PRODUCT_GUID "ca5bf4fe-7b38-4003-9455-de249d03caac")
	set(CPACK_PACKAGE_FILE_NAME "scap-x64-${OBS_VERSION}")
else()
	if(WIN32)
		set(CPACK_PACKAGE_NAME "Screen Recorder (32bit)")
	endif()
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "SCAP32")
	set(CPACK_WIX_UPGRADE_GUID "a26acea4-6190-4470-9fb9-f6d32f3ba030")
	set(CPACK_WIX_PRODUCT_GUID "8e24982d-b0ab-4f66-9c90-f726f3b64682")
	set(CPACK_PACKAGE_FILE_NAME "scap-x86-${OBS_VERSION}")
endif()

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

if(UNIX_STRUCTURE)
	set(CPACK_SET_DESTDIR TRUE)
endif()


if (WIN32)
	INSTALL(FILES ${DepsPath}/../bin/ffmpeg.exe DESTINATION "bin/64bit" PERMISSIONS OWNER_READ OWNER_EXECUTE)

	INSTALL(FILES ${CMAKE_BINARY_DIR}/bin/Release/QtAV.dll DESTINATION "bin/64bit")
	INSTALL(FILES ${CMAKE_BINARY_DIR}/bin/Release/QtAVWidgets.dll DESTINATION "bin/64bit")
	
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/UI/features/upload/data/uploadAccount.xml DESTINATION "bin/${_output_suffix}bit")
elseif (APPLE)
	INSTALL(FILES /usr/local/Cellar/ffmpeg/3.0.2/bin/ffmpeg DESTINATION "bin" PERMISSIONS OWNER_READ OWNER_EXECUTE)

	INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/lib/QtAV.framework DESTINATION "bin")
	INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/lib/QtAVWidgets.framework DESTINATION "bin")
	
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/UI/features/upload/data/uploadAccount.xml DESTINATION "bin")
endif ()


if (WIN32)
	INSTALL(FILES ${DepsPath}/../bin/openssl/libeay32.dll DESTINATION "bin/64bit")
	INSTALL(FILES ${DepsPath}/../bin/openssl/ssleay32.dll DESTINATION "bin/64bit")
endif ()



include(CPack)
