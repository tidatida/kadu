#cmakedefine DEBUG_ENABLED 1

#define KADU_DATADIR_RELATIVE_TO_BIN "${KADU_DATADIR_RELATIVE_TO_BIN}"
#define KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "${KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN}"
#define KADU_DESKTOP_FILE_PATH_RELATIVE_TO_BIN "${KADU_DESKTOP_FILE_DIR_RELATIVE_TO_BIN}/${KADU_DESKTOP_FILE_NAME}"

#define KADU_VERSION "${KADU_VERSION}"
#define KADU_NUMERIC_VERSION ${KADU_NUMERIC_VERSION}

#cmakedefine01 SIG_HANDLING_ENABLED
#cmakedefine01 HAVE_EXECINFO
#cmakedefine01 WITH_LIBINDICATE_QT
