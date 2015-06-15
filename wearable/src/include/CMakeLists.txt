SET(HEADER_FILES
	circle/efl_extension_circle_surface.h
	circle/efl_extension_circle_object.h
	circle/efl_extension_circle_object_progressbar.h
	circle/efl_extension_circle_object_slider.h
	circle/efl_extension_circle_object_genlist.h
	circle/efl_extension_circle_object_datetime.h
	circle/efl_extension_circle_object_scroller.h
	circle/efl_extension_rotary.h
	default/efl_extension_common.h
	default/efl_extension_more_option_layout.h
	)

SET(COMMON_HEADERS
	${COMMON_HEADERS}
	${HEADER_FILES}
	)

FOREACH(files ${COMMON_HEADERS})
	SET(HEADER_INCS "${HEADER_INCS}#include \"${files}\"\n")
ENDFOREACH(files)

CONFIGURE_FILE(
	${COMMON_DIR}/include/efl_extension.h.in ${COMMON_DIR}/include/efl_extension.h
	@ONLY)

SET(HEADER_FILES
	${HEADER_FILES}
	)

FOREACH(files ${HEADER_FILES})
	GET_FILENAME_COMPONENT(dir ${files} PATH)
	INSTALL(FILES ${files} DESTINATION include/${LIB_NAME}/${dir})
ENDFOREACH()
INSTALL(FILES ${COMMON_DIR}/include/efl_extension.h DESTINATION include/${LIB_NAME})
