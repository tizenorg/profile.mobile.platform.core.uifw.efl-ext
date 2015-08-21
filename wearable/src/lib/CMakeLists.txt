add_definitions(-DEFL_EO_API_SUPPORT=1 -DEFL_BETA_API_SUPPORT=1)

SET(LIB_SRCS
	${LIB_SRCS}
	circle/efl_extension_circle_surface.c
	circle/efl_extension_circle_object.c
	circle/efl_extension_circle_object_item.c
	circle/efl_extension_circle_object_progressbar.c
	circle/efl_extension_circle_object_slider.c
	circle/efl_extension_circle_object_genlist.c
	circle/efl_extension_circle_object_datetime.c
	circle/efl_extension_circle_object_scroller.c
	circle/efl_extension_rotary.c
	default/efl_extension_more_option_layout.c)
SET (ALL_EO_FILES
	)


foreach(eo_file ${ALL_EO_FILES})
	string(REPLACE .eo .eo.c out_c_file ${eo_file})
	string(REPLACE .eo .eo.h out_h_file ${eo_file})
	string(REPLACE .eo .eo.legacy.h out_legacy_file ${eo_file})
	string(REPLACE .eo .c c_file ${eo_file})
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${out_c_file}
		COMMAND /usr/bin/eolian_gen ${eo_file} --eo --legacy --gc -I /usr/share/eolian/include -o ${out_c_file}
		COMMAND /usr/bin/eolian_gen ${eo_file} --eo --gh -I /usr/share/eolian/include -o "../include/${out_h_file}"
		COMMAND /usr/bin/eolian_gen ${eo_file} --legacy --gh -I /usr/share/eolian/include -o "../include/${out_legacy_file}"
		DEPENDS ${eo_file} ${CMAKE_CURRENT_SOURCE_DIR}/*.eo
		COMMENT "Compiling Eo: ${eo_file}, Output eo : ${out_c_file}"
		VERBATIM)
	add_custom_target(target_${eo_file} ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${out_c_file})
endforeach(eo_file)

ADD_LIBRARY(${LIB_NAME} SHARED ${LIB_SRCS})

ADD_DEFINITIONS("-DEXPORT_API=__attribute__((visibility(\"default\")))")
PKG_CHECK_MODULES(LIB_PKGS REQUIRED
		elementary
		dlog
		eo
		ecore
		ecore-x
		elementary
		dlog
		cairo
		x11
		xext
		xi
		inputproto
		fontconfig
		glib-2.0
		)

FOREACH(flag ${LIB_PKGS_CFLAGS})
	SET(LIB_CFLAGS "${LIB_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES COMPILE_FLAGS "${LIB_CFLAGS}")
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES SOVERSION ${VERSION_MAJOR})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${LIB_PKGS_LDFLAGS} ${LIB_TARGET_PKGS_LDFLAGS})

INSTALL(TARGETS ${LIB_NAME} DESTINATION lib)
INSTALL(FILES ${ALL_EO_FILES} DESTINATION share/eolian/include/efl-extension)

