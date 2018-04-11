add_executable (wav2mp3
    main.cpp
    utils.cpp
    ConcurrentQueue.cpp
    FileEncodeJob.cpp
    EncoderThread.cpp
)

target_link_libraries(wav2mp3
    ${LAME_LIBRARIES}
    Threads::Threads
)

install(TARGETS wav2mp3
  RUNTIME DESTINATION bin
)
