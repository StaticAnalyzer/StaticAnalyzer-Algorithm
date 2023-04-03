add_executable(algServer server.cpp)

target_link_libraries(algServer
  algproto
  analysis
  stdc++fs
)
