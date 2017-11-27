add_executable(dbms_test
    main.cpp
    ColumnStoreTest.cpp
    CompressionTest.cpp
    RowStoreTest.cpp
    SchemaTest.cpp
    UtilTest.cpp
    )
target_link_libraries(dbms_test dbms impl)
add_dependencies(check dbms_test)
add_test(NAME dbms COMMAND dbms_test)
