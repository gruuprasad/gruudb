add_executable(stud_test
    main.cpp
# Nothing here yet.  Add your tests below.
    )
target_link_libraries(stud_test dbms impl)
add_dependencies(check stud_test)
add_test(NAME stud COMMAND stud_test)
