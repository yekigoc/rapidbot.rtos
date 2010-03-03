AC_INIT([rgi], [0.1], [snegovick@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror])

#AC_CHECK_FUNCS([gethostbyname])
#AC_CHECK_FUNCS([inet_ntoa])
#AC_CHECK_FUNCS([memset])
#AC_CHECK_FUNCS([select])
#AC_CHECK_FUNCS([socket])
#AC_CHECK_HEADERS([fcntl.h])
#AC_CHECK_HEADERS([netdb.h])
#AC_CHECK_HEADERS([netinet/in.h])
#AC_CHECK_HEADERS([sys/socket.h])
AC_PROG_CC
AC_FUNC_MALLOC
#AC_PROG_CXX
AC_PROG_RANLIB
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

GOB2_CHECK([2.0.0])

# glib's directory structure is dependent on which version of glib is installed.
# So, allow the user to specify the version,
# then allow them to specify the overall path, and munge things together so that they basically work right.
AC_ARG_WITH(glib-version, [  --with-glib-version=VERSION	Specify location of glib include files], [glibversion="glib-$withval"], [glibversion="glib-2.0"])

AC_ARG_WITH(glib, [  --with-glib=PREFIX	Specify location of glib-2.0], [glib="$withval"], [glib=yes])
if test "$glib" != no; then
   if test "$glib" != yes; then
      CFLAGS="$CFLAGS -I$glib/include -I$glib/include/$glibversion -I$glib/include/$glibversion/glib -I$glib/lib/$glibversion/include"
      LDFLAGS="$LDFLAGS -L$glib/lib"
   else
      CFLAGS="$CFLAGS -I/usr/include -I/usr/include/$glibversion -I/usr/include/$glibversion/glib -I/usr/lib/$glibversion/include"
   fi
   AC_CHECK_HEADERS([glib.h],,AC_MSG_ERROR([glib.h header not found]))
   AC_CHECK_LIB([glib-2.0], [g_option_context_new],,AC_MSG_ERROR([glib-2.0 library not found]))
else
   AC_MSG_ERROR(This package requires glib-2.0.)
fi

# check for gtk-doc
GTK_DOC_CHECK(1.9)
AC_CONFIG_MACRO_DIR(m4)


#AC_ARG_WITH(gio, [  --with-gio=PREFIX	Specify location of gio], [gio="$withval"], [gio=yes])
#if test "$gio" != no; then
#   if test "$gio" != yes; then
#      CFLAGS="$CFLAGS -I$gio/gio"
#      LDFLAGS="$LDFLAGS -L$gio/lib"
#   else
#      CFLAGS="$CFLAGS -I/usr/include -I/usr/include/$glibversion/gio"
#   fi
#   AC_CHECK_HEADERS([gio.h],,AC_MSG_ERROR([gio.h header not found]))
#   AC_CHECK_LIB([gio-2.0], [g_threaded_socket_service_new],,AC_MSG_ERROR([gio-2.0 library not found]))
#else
#   AC_MSG_ERROR(This package requires gio.)
#fi

#AC_ARG_WITH(gthread, [  --with-gthread=PREFIX	Specify location of gthread], [gthread="$withval"], [gthread=yes])
#if test "$gthread" != no; then
#   if test "$gthread" != yes; then
#      CFLAGS="$CFLAGS -I$gio/gio"
#      LDFLAGS="$LDFLAGS -L$gthread/lib"
#   else
#      CFLAGS="$CFLAGS -I/usr/include -I/usr/include/$glibversion/gio"
#  fi
#AC_CHECK_HEADERS([gio.h],,AC_MSG_ERROR([gio.h header not found]))
#AC_CHECK_LIB([gthread-2.0], [g_thread_init],,AC_MSG_ERROR([gthread-2.0 library not found]))
#AC_CHECK_LIB([gmodule-2.0], [g_module_open],,AC_MSG_ERROR([gmodule-2.0 library not found]))
#else
#   AC_MSG_ERROR(This package requires gio.)
#fi

#AC_CHECK_HEADERS([gst.h],,AC_MSG_ERROR([gio.h header not found]))
#AC_CHECK_LIB([gthread-2.0], [g_thread_init],,AC_MSG_ERROR([gthread-2.0 library not found]))
#AC_CHECK_LIB([gmodule-2.0], [g_module_open],,AC_MSG_ERROR([gmodule-2.0 library not found]))

#PKG_CHECK_MODULES([DEPS], gstreamer-0.10)
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
	lib/Makefile
	src/Makefile
])

#	doc/Makefile
#	libgasclient/Makefile
#	rgi_client/bin/Makefile
#	rgi_client/lib/Makefile
AC_OUTPUT