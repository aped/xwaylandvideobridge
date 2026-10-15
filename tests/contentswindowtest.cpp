/*
 * SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL
 * SPDX-FileCopyrightText: 2026 Andrew <aped@users.noreply.github.com>
 */

#include "contentswindow.h"

#include <QGuiApplication>
#include <QScopedPointer>
#include <QTest>
#include <QtGui/qguiapplication_platform.h>

#include <xcb/xcb.h>
#include <xcb/xfixes.h>

class ContentsWindowTest : public QObject
{
    Q_OBJECT

private:
    static xcb_connection_t *connection()
    {
        auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
        return x11 ? x11->connection() : nullptr;
    }

    static int boundingRectangleCount(WId window)
    {
        auto *xcb = connection();
        const xcb_xfixes_region_t region = xcb_generate_id(xcb);
        xcb_xfixes_create_region_from_window(xcb, region, window, XCB_SHAPE_SK_BOUNDING);

        QScopedPointer<xcb_xfixes_fetch_region_reply_t, QScopedPointerPodDeleter> reply(
            xcb_xfixes_fetch_region_reply(xcb, xcb_xfixes_fetch_region(xcb, region), nullptr));
        xcb_xfixes_destroy_region(xcb, region);

        return reply ? xcb_xfixes_fetch_region_rectangles_length(reply.data()) : -1;
    }

    static uint8_t mapState(WId window)
    {
        auto *xcb = connection();
        QScopedPointer<xcb_get_window_attributes_reply_t, QScopedPointerPodDeleter> reply(
            xcb_get_window_attributes_reply(xcb, xcb_get_window_attributes(xcb, window), nullptr));
        return reply ? reply->map_state : uint8_t(XCB_MAP_STATE_UNMAPPED);
    }

private Q_SLOTS:
    void idleWindowHasNoBoundingPixels()
    {
        if (!connection()) {
            QSKIP("This test requires the xcb platform plugin");
        }

        ContentsWindow window;
        window.show();
        QTRY_COMPARE(mapState(window.winId()), uint8_t(XCB_MAP_STATE_VIEWABLE));
        QVERIFY(!window.size().isEmpty());
        QCOMPARE(boundingRectangleCount(window.winId()), 0);

        window.showForStream(QSize(320, 180));
        QTRY_VERIFY(boundingRectangleCount(window.winId()) > 0);

        window.goIdle();
        QTRY_COMPARE(boundingRectangleCount(window.winId()), 0);
        QCOMPARE(mapState(window.winId()), uint8_t(XCB_MAP_STATE_VIEWABLE));
        QVERIFY(!window.size().isEmpty());
    }
};

QTEST_MAIN(ContentsWindowTest)

#include "contentswindowtest.moc"
