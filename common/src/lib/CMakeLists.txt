add_definitions(-DEFL_EO_API_SUPPORT=1 -DEFL_BETA_API_SUPPORT=1)

SET(LIB_SRCS
	${COMMON_DIR}/lib/efl_extension.c
	${COMMON_DIR}/lib/eext_events.c
	CACHE
	INTERNAL
	"LIB_SRCS"
	)
