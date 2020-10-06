set(args "")
foreach(n RANGE ${CMAKE_ARGC})
  list(APPEND args ${CMAKE_ARGV${n}})
endforeach()

list(FIND args "--" index)
if(index EQUAL -1)
  message(FATAL_ERROR "No -- divider found in arguments list")
else()
  set(temp "${args}")
  math(EXPR index "${index} + 1")
  list(SUBLIST temp ${index} -1 args)
endif()

list(POP_FRONT args source build cmake ninja cores)

include(cmake/exec.cmake)

exec(${cmake} -S ${source} -B ${build} -G Ninja -D CMAKE_MAKE_PROGRAM=${ninja}
        -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=.install ${args})

exec(${cmake} --build ${build} -j ${cores})
