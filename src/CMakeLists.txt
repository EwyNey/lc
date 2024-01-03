add_executable(main main.cpp)

# add_executable(lexer lexer.cpp)

# target_link_libraries(lexer ${llvm_libs})



target_link_libraries(main ${llvm_libs})

add_executable(main2 main2.cpp)

