set (os_SRCS
	generic/compositing-aware-object.cpp
	generic/desktop-aware-object.cpp
	generic/desktop-aware-object-helper.cpp
	generic/system-info.cpp
	generic/url-opener.cpp
	generic/window-geometry-manager.cpp
	win/wsa-handler.cpp
	os-module.cpp
)

if (NOT WIN32)
	list (APPEND os_SRCS
		unix/unix-signal-handler.cpp
	)
endif ()

set (os_MOC_SRCS
	generic/desktop-aware-object-helper.h
	generic/window-geometry-manager.h
)

if (MINGW)
	set_property (SOURCE generic/system-info.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
endif ()

kadu_subdirectory (os "${os_SRCS}" "${os_MOC_SRCS}" "")
