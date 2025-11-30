include(FetchContent)
include(DependenciesSHAs)
include(CompileGr4Release)

FetchContent_Declare(
  gnuradio4
  GIT_REPOSITORY https://github.com/fair-acc/gnuradio4.git
  GIT_TAG ${GIT_SHA_GNURADIO4}
  EXCLUDE_FROM_ALL SYSTEM)

FetchContent_MakeAvailable(
  gnuradio4)

od_set_release_flags_on_gnuradio_targets("${gnuradio4_SOURCE_DIR}")

