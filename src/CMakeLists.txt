cmake_minimum_required(VERSION 3.18)

set(PROJECT_NAME wolf3d_lives)
project(${PROJECT_NAME})

set(VENDOR_ROOT libs)
set(JPEGLIB_ROOT ${VENDOR_ROOT}/jpeglib)

include_directories(msvc_compat ${JPEGLIB_ROOT})

########################
## Wolf3DLives sources #
########################
set(WOLF3DLIVES_SOURCES
	actor_ai.c
	actors.c
	ai_com.c
	areas.c
	automap.c
	cl_input.c
	cl_main.c
	console.c
	doors.c
	fileio.c
	fmopl.c
	fontman.c
	game.c
	gl_draw.c
	keys.c
	level.c
	map.c
	math.c
	menu.c
	opengl.c
	particles.c
	player.c
	powerups.c
	pwalls.c
	q_fns.c
	r_models.c
	raycast.c
	render.c
	saveload.c
	sound.c
	sprites.c
	texman.c
	version.c
	video.c
	wl_text.c
	wolfgl.c    
)

set(WOLF3DLIVES_COMMON_SOURCES
	common/cmd.c
	common/common.c
	common/common.h
	common/cvar.c
	common/files.c
	common/io_images.c
	common/keys.h
	common/parselib.c
)
source_group("Source Files\\common" FILES ${WOLF3DLIVES_COMMON_SOURCES})

set(WOLF3DLIVES_WIN32_SOURCES
	win32/afxres.h
	win32/gl_win.c
	win32/in_win.c
	win32/resource.h
	win32/sys_info.c
	win32/sys_win.c
	win32/vid_win.c
	win32/win32.h
)
source_group("Source Files\\win32" FILES ${WOLF3DLIVES_WIN32_SOURCES})

set(WOLF3DLIVES_REFRESH_SOURCES
	r_gl/glext.h
	r_gl/r_gl.c
	r_gl/r_gl.h
)
source_group("Source Files\\r_gl" FILES ${WOLF3DLIVES_REFRESH_SOURCES})

set(WOLF3DLIVES_SCRIPTS_SOURCES
	scripts/scr_models.c
	scripts/scripts.h
)
source_group("Source Files\\scripts" FILES ${WOLF3DLIVES_SCRIPTS_SOURCES})

set(WOLF3DLIVES_HEADER_FILES
	act_stat.h
	actor_ai.h
	actors.h
	ai_com.h
	anorms.h
	areas.h
	audiowl6.h
	automap.h
	build.h
	client.h
	console.h
	doors.h
	draw.h
	fileio.h
	fileioo.h
	fmopl.h
	fontman.h
	game.h
	keys.h
	level.h
	map.h
	math.h
	menu.h
	menudef.h
	opengl.h
	particles.h
	player.h
	powerups.h
	pwalls.h
	q_fns.h
	r_models.h
	raycast.h
	render.h
	saveload.h
	sound.h
	sprites.h
	sprt_def.h
	texman.h
	typ.h
	version.h
	vgapics.h
	video.h
	wl_ai.h
	wl_def.h
	wl_game.h
	wl_text.h
	wluser.h
	wolfdef.h
	wolfgl.h
)
source_group("Header Files" FILES ${WOLF3DLIVES_HEADER_FILES})

set(WOLF3DLIVES_RESOURCE_FILES
	win32/newwolf.rc
)
source_group("Resource Files" FILES ${WOLF3DLIVES_RESOURCE_FILES})

###################
# jpeglib sources #
###################
set(WOLF3DLIVES_LIBS_JPEGLIB_SOURCES
	${JPEGLIB_ROOT}/jcapimin.c
	${JPEGLIB_ROOT}/jcapistd.c
	${JPEGLIB_ROOT}/jccoefct.c
	${JPEGLIB_ROOT}/jccolor.c
	${JPEGLIB_ROOT}/jcdctmgr.c
	${JPEGLIB_ROOT}/jchuff.c
	${JPEGLIB_ROOT}/jcinit.c
	${JPEGLIB_ROOT}/jcmainct.c
	${JPEGLIB_ROOT}/jcmarker.c
	${JPEGLIB_ROOT}/jcmaster.c
	${JPEGLIB_ROOT}/jcomapi.c
	${JPEGLIB_ROOT}/jcparam.c
	${JPEGLIB_ROOT}/jcphuff.c
	${JPEGLIB_ROOT}/jcprepct.c
	${JPEGLIB_ROOT}/jcsample.c
	${JPEGLIB_ROOT}/jctrans.c
	${JPEGLIB_ROOT}/jdapimin.c
	${JPEGLIB_ROOT}/jdapistd.c
	${JPEGLIB_ROOT}/jdatadst.c
	${JPEGLIB_ROOT}/jdatasrc.c
	${JPEGLIB_ROOT}/jdcoefct.c
	${JPEGLIB_ROOT}/jdcolor.c
	${JPEGLIB_ROOT}/jddctmgr.c
	${JPEGLIB_ROOT}/jdhuff.c
	${JPEGLIB_ROOT}/jdinput.c
	${JPEGLIB_ROOT}/jdmainct.c
	${JPEGLIB_ROOT}/jdmarker.c
	${JPEGLIB_ROOT}/jdmaster.c
	${JPEGLIB_ROOT}/jdmerge.c
	${JPEGLIB_ROOT}/jdphuff.c
	${JPEGLIB_ROOT}/jdpostct.c
	${JPEGLIB_ROOT}/jdsample.c
	${JPEGLIB_ROOT}/jdtrans.c
	${JPEGLIB_ROOT}/jerror.c
	${JPEGLIB_ROOT}/jfdctflt.c
	${JPEGLIB_ROOT}/jfdctfst.c
	${JPEGLIB_ROOT}/jfdctint.c
	${JPEGLIB_ROOT}/jidctflt.c
	${JPEGLIB_ROOT}/jidctfst.c
	${JPEGLIB_ROOT}/jidctint.c
	${JPEGLIB_ROOT}/jidctred.c
	${JPEGLIB_ROOT}/jmemmgr.c
	${JPEGLIB_ROOT}/jmemnobs.c
	${JPEGLIB_ROOT}/jquant1.c
	${JPEGLIB_ROOT}/jquant2.c
	${JPEGLIB_ROOT}/jutils.c
)
source_group("Source Files\\libs\\jpeglib" FILES ${WOLF3DLIVES_LIBS_JPEGLIB_SOURCES})

set(WOLF3DLIVES_LIBS_JPEGLIB_HEADERS_SOURCES
	${JPEGLIB_ROOT}/jchuff.h
	${JPEGLIB_ROOT}/jconfig.h
	${JPEGLIB_ROOT}/jdct.h
	${JPEGLIB_ROOT}/jdhuff.h
	${JPEGLIB_ROOT}/jerror.h
	${JPEGLIB_ROOT}/jinclude.h
	${JPEGLIB_ROOT}/jmemsys.h
	${JPEGLIB_ROOT}/jmorecfg.h
	${JPEGLIB_ROOT}/jpegint.h
	${JPEGLIB_ROOT}/jpeglib.h
	${JPEGLIB_ROOT}/jversion.h
)
source_group("Source Files\\libs\\jpeglib\\headers" FILES ${WOLF3DLIVES_LIBS_JPEGLIB_HEADERS_SOURCES})

##################################
# Targets and compile/link flags #
##################################
set(WOLF3DLIVES_ALL_SOURCES
	${WOLF3DLIVES_SOURCES}
	${WOLF3DLIVES_COMMON_SOURCES}
	${WOLF3DLIVES_WIN32_SOURCES}
	${WOLF3DLIVES_REFRESH_SOURCES}
	${WOLF3DLIVES_SCRIPTS_SOURCES}
	${WOLF3DLIVES_LIBS_JPEGLIB_SOURCES}
	${WOLF3DLIVES_LIBS_JPEGLIB_HEADERS_SOURCES}

	# Include headers if they are to appear in IDE
	${WOLF3DLIVES_HEADER_FILES}
)

add_executable(${PROJECT_NAME} WIN32
		${WOLF3DLIVES_ALL_SOURCES}
		${WOLF3DLIVES_RESOURCE_FILES})

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL /MT")

set(CMAKE_EXE_LINKER_FLAGS_RELEASE
	"${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /LTCG /OPT:REF")

set_target_properties(${PROJECT_NAME}
	PROPERTIES
		COMPILE_DEFINITIONS
			_CRT_SECURE_NO_DEPRECATE
			DEBUG_OUTPUT_NAME ${PROJECT_NAME}_d
)

target_link_libraries(${PROJECT_NAME} OpenGL32 Glu32 Winmm)

