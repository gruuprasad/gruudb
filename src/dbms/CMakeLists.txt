add_library(
    dbms
    Loader.cpp
    )

add_executable(load load.cpp)
target_link_libraries(load dbms impl)

add_executable(benchmark_stores benchmark_stores.cpp)
target_link_libraries(benchmark_stores dbms impl)
