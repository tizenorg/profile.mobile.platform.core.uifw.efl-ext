SET(HEADER_FILES
	eext_floatingbutton.h
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
	eext_floatingbutton_common.h
	eext_floatingbutton_legacy.h
	eext_floatingbutton.eo.legacy.h
	eext_floatingbutton_eo.h
	eext_floatingbutton.eo.h
	)

FOREACH(files ${HEADER_FILES})
	GET_FILENAME_COMPONENT(dir ${files} PATH)
	INSTALL(FILES ${files} DESTINATION include/${LIB_NAME}/${dir})
ENDFOREACH()
INSTALL(FILES ${COMMON_DIR}/include/efl_extension.h DESTINATION include/${LIB_NAME})
