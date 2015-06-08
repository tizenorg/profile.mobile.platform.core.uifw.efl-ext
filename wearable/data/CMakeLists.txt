SET(EDC_FILES
	)

FOREACH(edc ${EDC_FILES})
	SET(EDC_INCS "${EDC_INCS}\n   #include \"${edc}\"")
ENDFOREACH(edc)

CONFIGURE_FILE(
	efl-extension.edc.in efl-extension.edc
	@ONLY)

ADD_CUSTOM_TARGET(${PROJECT_NAME}.edj
   COMMAND edje_cc -id ./images/ ${PROJECT_NAME}.edc ${PROJECT_NAME}.edj
)
ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}.edj)

INSTALL(FILES ${PROJECT_NAME}.edj DESTINATION ${EDJDIR})
