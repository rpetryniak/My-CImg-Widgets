cmake_minimum_required(VERSION 2.6)

PROJECT (MyCImgWidgets)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} include )

SET( GUI_IMAGE_SRCS
  src/gui/MyImageCanvas.cpp
  src/gui/MyImageWindow.cpp
)

SET( GUI_LIST_SRCS
  src/gui/MyImageListCanvas.cpp
  src/gui/MyImageList.cpp
  src/gui/MyImageListWindow.cpp
)

SET ( DB_SRCS
  src/db/MyImageDB.cpp
)
SET ( QUERY_SRCS
  src/db/MyImageQuery.cpp
)

ADD_EXECUTABLE(image_viewer app/image_viewer.cpp ${GUI_IMAGE_SRCS} )
ADD_EXECUTABLE(image_list_viewer app/image_list_viewer.cpp ${GUI_IMAGE_SRCS} ${GUI_LIST_SRCS})
ADD_EXECUTABLE(image_db_viewer app/image_db_viewer.cpp ${GUI_IMAGE_SRCS} ${GUI_LIST_SRCS} ${DB_SRCS})
ADD_EXECUTABLE(image_list_query app/image_list_query.cpp ${GUI_IMAGE_SRCS} ${GUI_LIST_SRCS} ${QUERY_SRCS} )
ADD_EXECUTABLE(image_db_query app/image_db_query.cpp ${GUI_IMAGE_SRCS} ${GUI_LIST_SRCS} ${DB_SRCS} ${QUERY_SRCS} )

TARGET_LINK_LIBRARIES(image_viewer X11 pthread m )
TARGET_LINK_LIBRARIES(image_list_viewer X11 pthread m )
TARGET_LINK_LIBRARIES(image_db_viewer X11 pthread m sqlite3)
TARGET_LINK_LIBRARIES(image_list_query X11 pthread m )
TARGET_LINK_LIBRARIES(image_db_query X11 pthread m sqlite3)
