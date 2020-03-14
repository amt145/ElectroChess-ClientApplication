#pragma once
#include "ElectroWin.h"
#include "Vec2.h"

template <class T>
class Rect
{
public:
	Rect() : topL(0, 0), topR(1, 0), bottomR(1, 1), bottomL(0, 1) {};
	Rect(T x, T y) : topL(0, 0), topR(x, 0), bottomR(x, y), bottomL(0, y) {};
	Rect(Vec2<T> v) : topL(0, 0), topR(v.x(), 0), bottomR(v.x(), v.y()), bottomL(0, v.y()) {};
	Rect(T x1, T y1, T x2, T y2) : topL(x1, y1), topR(x2, y1), bottomR(x2, y2), bottomL(x1, y2) {};
	Rect(Vec2<T> v1, Vec2<T> v2) : topL(v1.x(), v1.y()), topR(v2.x(), v1.y()), bottomR(v2.x(), v2.y()), bottomL(v1.x(), v2.y()) {};

	~Rect() = default;
	Rect(const Rect& rhs) = default;
	Rect(Rect&& rhs) = default;
	Rect& operator=(const Rect& rhs) = default;
	Rect& operator=(Rect&& rhs) = default;
	/*
	Rect(const Rect& rhs) {
		topL = rhs.topL;
		topR = rhs.topR;
		bottomR = rhs.bottomR;
		bottomL = rhs.bottomL;
	}
	Rect(Rect&& rhs);
	Rect& operator=(const Rect& rhs) {
		topL = rhs.topL;
		topR = rhs.topR;
		bottomR = rhs.bottomR;
		bottomL = rhs.bottomL;
	}
	Rect& operator=(Rect&& rhs);
	*/
	Vec2<T> GetTopLeft() const { return topL; };
	Vec2<T> GetTopRight() const { return topR; };
	Vec2<T> GetBottomRight() const { return bottomR; };
	Vec2<T> GetBottomLeft() const { return bottomL; };

	void SetTopLeft(Vec2<T> x) { topL = x; };
	void SetTopRight(Vec2<T> x) { topR = x; };
	void SetBottomLeft(Vec2<T> x) { bottomL = x; };
	void SetBottomRight(Vec2<T> x) { bottomR = x; };
private:
	Vec2<T> topL;
	Vec2<T> topR;
	Vec2<T> bottomR;
	Vec2<T> bottomL;
};

typedef Rect<short> SHORT_RECT;