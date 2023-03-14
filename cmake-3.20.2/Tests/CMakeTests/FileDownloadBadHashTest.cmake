if(NOT "/home/zearaujo11/Programming/ia_t03g34_assignment1/cmake-3.20.2/Tests/CMakeTests" MATCHES "^/")
  set(slash /)
endif()
set(url "file://${slash}/home/zearaujo11/Programming/ia_t03g34_assignment1/cmake-3.20.2/Tests/CMakeTests/FileDownloadInput.png")
set(dir "/home/zearaujo11/Programming/ia_t03g34_assignment1/cmake-3.20.2/Tests/CMakeTests/downloads")

file(DOWNLOAD
  ${url}
  ${dir}/file3.png
  TIMEOUT 2
  STATUS status
  EXPECTED_HASH SHA1=5555555555555555555555555555555555555555
  )
