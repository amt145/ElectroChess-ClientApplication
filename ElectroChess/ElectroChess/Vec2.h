#pragma once
#include "ElectroWin.h"

template<class T>
class Vec2
{
public:
	Vec2() : _x(0), _y(0) {};
	Vec2(T x) : _x(x), _y(0) {};
	Vec2(T x, T y) : _x(x), _y(y) {};
	Vec2(COORD p) : _x(p.X), _y(p.Y) {};
	Vec2(const Vec2& rhs) = default;
	Vec2& operator=(const Vec2& rhs) = default;
	~Vec2() = default;
	Vec2(Vec2&& rhs) = default;
	Vec2& operator=(Vec2&& rhs) = default;
	T x() const { return _x; };
	T y() const { return _y; };
	void SetX(T dat) { _x = dat; };
	void SetY(T dat) { _y = dat; };

	bool operator==(const Vec2& rhs) { return (_x == rhs._x && _y == rhs._y); };
	bool operator!=(const Vec2& rhs) { return !(*this == rhs); };
private:
	T _x;
	T _y;
};
typedef Vec2<int> v2i;
typedef Vec2<short> v2s;
typedef Vec2<double> v2d;
typedef Vec2<float> v2f;