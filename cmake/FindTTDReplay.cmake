############################################
# Use to find dll of Time Travel Debugging #
############################################
# OUTPUT is ${TTDReplay_INSTALL_PATH} set with the correct path
# INSTALL_TTDREPLAY macro use to copy ttd dll files

find_path(
	TTDReplay_INSTALL_PATH
	NAMES TTDReplay.dll
	PATHS "${WindbgPreview_INSTALL_PATH}/amd64/ttd"
	NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    TTDReplay
	REQUIRED_VARS TTDReplay_INSTALL_PATH
    FAIL_MESSAGE "
#######################################################
Could not find Time Travel Debugging dll. Make sure you installed windbg preview edition or disable using -DINSTALL_TTDDLL=OFF
#######################################################
")

macro(INSTALL_TTDREPLAY DESTINATION)

install(FILES "${TTDReplay_INSTALL_PATH}/TTDReplay.dll" DESTINATION ${DESTINATION})
install(FILES "${TTDReplay_INSTALL_PATH}/TTDReplayCPU.dll" DESTINATION ${DESTINATION})

endmacro(INSTALL_TTDREPLAY)
