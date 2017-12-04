

cmake_minimum_required(VERSION 3.4.1)

SET(distribution_DIR ../../../../libs)

add_library( # Sets the name of the library.
             ffmpeg-lib
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/ffmpeg_player.c )


find_library(avcodec-56-lib
             SHARED
             IMPORTTED )

set_target_properties( avcodec-56-lib
                        PROPERTIES IMPORTED_LOCATION
                        ${distribution_DIR}/armeabi/libavcodec-56 )



find_library(avdevice-56-lib
            SHARED
            IMPROTED )

set_target_properties( avdevice-56-lib
                        PROPERTIES IMPROTEC_LOCATION
                        ${distribution_DIR}/armeabi/libavdevice-56.so )


find_library(avfilter-5-lib
             SHARED
             IMPROTED )

set_target_properties( avfilter-5-lib
                       PROPERTIES IMPROTED_LOCATION
                       ${distribution_DIR}/armeabi/libavfilter-5.so )


find_library(avformat-56-lib
             SHARED
             IMPROTED )

set_target_properties(avformat-lib
                      PROPERTIES IMPROTED_LOCATION
                      ${distribution_DIR}/armeabi/libavformat-56.so )


find_library(avutil-54-lib
             SHARED
             IMPROTED )

set_target_properties(avutil-54-lib
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libavutil-54.so )


find_library(postproc-53-lib
             SHARED
             IMPROTED )

set_target_properties(postproc-53-lib
                      PROPERTIES IMPROTED_LOCATION
                      ${distribution_DIR}/armeabi/libpostproc-53.so )


find_library(swresample-1-lib
             SHARED
             IMPROTED)

set_target_properties(swresample-1-lib
                      PROPERTIES IMPROTED_LOCATION
                      ${distribution_DIR}/armeabi/libswresample-1.so)


find_library(swscale-3-lib
             SHARED
             IMPROTED )

set_target_properties(swscale-3-lib
                      PROPERTIES IMPROTED_LOCATION
                      ${distribution_DIR}/armeabi/libswscale-3.so)



find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )



target_link_libraries( # Specifies the target library.
                       ffmpeg-lib-lib
                       avcodec-56-lib
                       avdevice-56-lib
                       avfilter-5-lib
                       avformat-56-lib
                       avutil-54-lib
                       postproc-53-lib
                       swresample-1-lib
                       swscale-3-lib



                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )