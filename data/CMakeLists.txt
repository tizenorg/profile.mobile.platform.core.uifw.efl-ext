ADD_CUSTOM_TARGET(efl-extension.edj
	COMMAND edje_cc -id ./images/ efl-extension.edc efl-extension.edj
)
ADD_DEPENDENCIES(${PROJECT_NAME} efl-extension.edj)

INSTALL(FILES efl-extension.edj DESTINATION share/efl-extension/themes)
