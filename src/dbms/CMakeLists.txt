add_library(
    dbms
    Loader.cpp
    )

add_executable(load load.cpp)
target_link_libraries(load dbms impl)

add_executable(benchmark_stores benchmark_stores.cpp)
target_link_libraries(benchmark_stores dbms impl)

add_executable(benchmark_compression benchmark_compression.cpp)
target_link_libraries(benchmark_compression dbms impl)

add_executable(load_compressed load_compressed.cpp)
target_link_libraries(load_compressed dbms impl)

add_executable(benchmark_indices benchmark_indices.cpp)
target_link_libraries(benchmark_indices dbms impl)
