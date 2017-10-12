add_executable(dbms_test
    main.cpp
    ColumnStoreTest.cpp
    RowStoreTest.cpp
    SchemaTest.cpp
    )
target_link_libraries(dbms_test dbms impl)
add_dependencies(check dbms_test)
add_test(NAME dbms COMMAND dbms_test)
