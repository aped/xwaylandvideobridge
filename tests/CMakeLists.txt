# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2026 Andrew <aped@users.noreply.github.com>

add_executable(contentswindowtest
    contentswindowtest.cpp
    ../src/contentswindow.cpp
)

target_include_directories(contentswindowtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(contentswindowtest PRIVATE
    Qt6::Quick
    Qt6::Test
    KF6::I18n
    KF6::WindowSystem
    XCB::XCB
    XCB::XFIXES
)

add_test(NAME contentswindowtest COMMAND contentswindowtest)
set_tests_properties(contentswindowtest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=xcb")
