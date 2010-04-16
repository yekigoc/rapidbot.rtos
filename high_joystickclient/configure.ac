AC_INIT([rgi], [0.1], [snegovick@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror])


AC_PROG_CC
AC_FUNC_MALLOC
#AC_PROG_CXX
AC_PROG_RANLIB
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T


AC_CONFIG_MACRO_DIR(m4)


AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)
#AC_MSG_CHECKING([gstreamer presence])
AC_MSG_RESULT($DEPS_CFLAGS)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_LIBTOOL
AC_PROG_CC
AM_PROG_CC_C_O
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

#	doc/Makefile
#	libgasclient/Makefile
#	rgi_client/bin/Makefile
#	rgi_client/lib/Makefile
AC_OUTPUT