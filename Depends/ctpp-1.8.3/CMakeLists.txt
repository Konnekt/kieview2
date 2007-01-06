PROJECT(CTPP1)

# ################################################################################
# Libraries
#
INCLUDE_DIRECTORIES(include)
# LINK_DIRECTORIES(lib) --- Broken in current cmake ----

SET(CMAKE_CXX_FLAGS "${CXX_FLAGS} -Wall -O3 -fomit-frame-pointer -funroll-loops -pedantic -Wno-long-long")
SET(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS_DEBUG} -Wall -g ")

# ################################################################################
# Sources
#

SET(LIBSRCS src/ctpp/ctpp.cpp
            src/ctpp/ctpp_c.cpp
            src/ctpp/param_data.cpp
            src/ctpp/udf_fn.cpp
            src/functions/form_param.cpp
            src/functions/href_param.cpp
            src/functions/htmlescape.cpp
            src/functions/num_format.cpp
            src/functions/std_fn_list.cpp
            src/functions/udf_is_a.cpp
            src/functions/urlescape.cpp
            src/functions/value_in_set.cpp
            src/functions/istrue.cpp
            )

ADD_LIBRARY(ctpp-static STATIC ${LIBSRCS})
SET_TARGET_PROPERTIES(ctpp-static PROPERTIES OUTPUT_NAME ctpp-st)
SET_TARGET_PROPERTIES(ctpp-static PROPERTIES LINKER_LANGUAGE CXX)

ADD_LIBRARY(ctpp SHARED ${LIBSRCS})
SET_TARGET_PROPERTIES(ctpp PROPERTIES OUTPUT_NAME ctpp)
SET_TARGET_PROPERTIES(ctpp PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(ctpp PROPERTIES VERSION 1.8.3 SOVERSION 1)

# ################################################################################
# Executable binary
#SET(LIBRARY_OUTPUT_PATH lib CACHE PATH "Built libraries are placed here before installation.") --- Broken in current cmake ----

# Install Headers
INSTALL(FILES include/ctpp/Hash.hpp
              include/ctpp/Types.h
              include/ctpp/ctpp.hpp
              include/ctpp/ctpp_c.h
              include/ctpp/param_data.hpp
              include/ctpp/udf_fn.hpp
        DESTINATION include/ctpp/ctpp)

INSTALL(FILES include/util/d2str.hpp
        DESTINATION include/ctpp/util)

INSTALL(TARGETS ctpp ctpp-static DESTINATION lib)
