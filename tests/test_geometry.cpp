/**
 * test_geometry.cpp
 *
 * Unit tests for geometry classes
 */

#include <QtTest/QtTest>
#include "../src/geometry/Point2D.h"
#include "../src/geometry/Line.h"
#include "../src/geometry/Circle.h"

using namespace PatternCAD::Geometry;

class GeometryTest : public QObject
{
    Q_OBJECT

private slots:
    void test_Point2D_distance();
    void test_Line_length();
    void test_Circle_area();
};

void GeometryTest::test_Point2D_distance()
{
    Point2D p1(0, 0);
    Point2D p2(3, 4);

    double distance = p1.distanceTo(p2);
    QCOMPARE(distance, 5.0);
}

void GeometryTest::test_Line_length()
{
    Line line(QPointF(0, 0), QPointF(3, 4));

    double length = line.length();
    QCOMPARE(length, 5.0);
}

void GeometryTest::test_Circle_area()
{
    Circle circle(QPointF(0, 0), 10.0);

    double area = circle.area();
    QVERIFY(qAbs(area - 314.159) < 0.01);
}

QTEST_MAIN(GeometryTest)
#include "test_geometry.moc"
