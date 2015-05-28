IF("${TIZEN_PROFILE_NAME}" STREQUAL "wearable")
	SET(HEADER_FILES
		eext_events.h
		circle/efl_extension_circle_surface.h
		circle/efl_extension_circle_object.h
		circle/efl_extension_circle_object_progressbar.h
		circle/efl_extension_circle_object_slider.h
		circle/efl_extension_circle_object_genlist.h
		circle/efl_extension_circle_object_datetime.h
		circle/efl_extension_circle_object_scroller.h
		circle/efl_extension_rotary.h
		common/efl_extension_common.h
		common/efl_extension_more_option_layout.h)
ELSE()
	SET(HEADER_FILES
		eext_events.h
		eext_floatingbutton.h)
ENDIF()

FOREACH(files ${HEADER_FILES})
	SET(HEADER_INCS "${HEADER_INCS}#include \"${files}\"\n")
ENDFOREACH(files)

CONFIGURE_FILE(
	efl_extension.h.in efl_extension.h
	@ONLY)

IF("${TIZEN_PROFILE_NAME}" STREQUAL "wearable")
	SET(HEADER_FILES
		${HEADER_FILES}
		efl_extension.h
		)
ELSE()
	SET(HEADER_FILES
		${HEADER_FILES}
		eext_floatingbutton_common.h
		eext_floatingbutton_legacy.h
		eext_floatingbutton.eo.legacy.h
		eext_floatingbutton_eo.h
		eext_floatingbutton.eo.h
		efl_extension.h
		)
ENDIF()

FOREACH(files ${HEADER_FILES})
	GET_FILENAME_COMPONENT(dir ${files} PATH)
	INSTALL(FILES ${files} DESTINATION include/${LIB_NAME}/${dir})
ENDFOREACH()
