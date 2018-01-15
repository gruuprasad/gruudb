add_executable(dbms_test
    main.cpp
    BPlusTreeTest.cpp
    ColumnStoreTest.cpp
    CompressionTest.cpp
    HashTableTest.cpp
    RowStoreTest.cpp
    SchemaTest.cpp
    UtilTest.cpp
    )
target_link_libraries(dbms_test dbms impl)
add_dependencies(check dbms_test)
add_test(NAME dbms COMMAND dbms_test)
