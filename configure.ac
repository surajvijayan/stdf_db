AC_INIT([stdf_db], [1.0], [suraj.vijayan1966@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_CHECK_LIB([mysqlcppconn], [main], [], [AC_MSG_ERROR([MySQL Connector/C++ not found])])
AC_CHECK_LIB([stdf], [main], [], [AC_MSG_ERROR([libstdf not found])])
AC_CHECK_LIB([config++], [main], [], [AC_MSG_ERROR([libconfig++ not found])])
AC_CHECK_LIB([crypto], [main], [], [AC_MSG_ERROR([OpenSSL crypto not found])])
AC_CHECK_LIB([pthread], [main], [], [AC_MSG_ERROR([pthread not found])])
AC_OUTPUT
