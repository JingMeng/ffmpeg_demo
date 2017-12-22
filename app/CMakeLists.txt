

cmake_minimum_required(VERSION 3.4.1)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

SET(distribution_DIR ../../../../libs)


add_library( # Sets the name of the library.
             # myffmpeg
             myplayer
             SHARED

             # Provides a relative path to your source file(s).
             # src/main/cpp/ffmpeg_decoder.c
             # src/main/cpp/my_player.c
             # src/main/cpp/audio_player.c
              src/main/cpp/audio_video_player.c
              src/main/cpp/play_sound_on_audiotrack.c)


add_library(avcodec-56
             SHARED
             IMPORTED )

set_target_properties( avcodec-56
                        PROPERTIES IMPORTED_LOCATION
                        ${distribution_DIR}/armeabi/libavcodec-56.so )



add_library(avdevice-56
            SHARED
            IMPORTED )

set_target_properties( avdevice-56
                        PROPERTIES IMPORTED_LOCATION
                        ${distribution_DIR}/armeabi/libavdevice-56.so )


add_library(avfilter-5
             SHARED
             IMPORTED )

set_target_properties( avfilter-5
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libavfilter-5.so )


add_library(avformat-56
             SHARED
             IMPORTED )

set_target_properties(avformat-56
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libavformat-56.so )


add_library(avutil-54
             SHARED
             IMPORTED )

set_target_properties(avutil-54
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libavutil-54.so )


add_library(postproc-53
             SHARED
             IMPORTED )

set_target_properties(postproc-53
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libpostproc-53.so )


add_library(swresample-1
             SHARED
             IMPORTED )

set_target_properties(swresample-1
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libswresample-1.so)


add_library(swscale-3
             SHARED
             IMPORTED )

set_target_properties(swscale-3
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libswscale-3.so)

add_library(yuv
            SHARED
            IMPORTED )

set_target_properties(yuv
                      PROPERTIES IMPORTED_LOCATION
                      ${distribution_DIR}/armeabi/libyuv.so)



include_directories(src/main/cpp/ffmpeg/include)
include_directories(src/main/cpp/libyuv/include)

target_link_libraries( # Specifies the target library.
                      # myffmpeg
                       myplayer
                       -ljnigraphics
                       -landroid # Add this.
                       yuv
                       avcodec-56
                       avdevice-56
                       avfilter-5
                       avformat-56
                       avutil-54
                       postproc-53
                       swresample-1
                       swscale-3



                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )