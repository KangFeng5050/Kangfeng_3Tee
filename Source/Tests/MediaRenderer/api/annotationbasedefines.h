#ifndef RT_AnnotationBaseDefines_H_
#define RT_AnnotationBaseDefines_H_


#include "list"

namespace tee3 {
	namespace avd{
	/* 点对象信息结构.
	*/
	typedef struct tagPoint {
		int x;
		int y;
		tagPoint() :x(0), y(0) {}
		tagPoint(int x, int y) :x(x), y(y) {}
	}Point;

	typedef std::list<Point> PointArray;

	/**
	* 区域对象信息结构.
	*/
	typedef struct tagRect {
		int    left;
		int    top;
		int    right;
		int    bottom;
		tagRect() :left(0), top(0), right(0), bottom(0) {}
		tagRect(int left, int top, int right, int bottom) :left(left), top(top), right(right), bottom(bottom) {}
		bool isRectEmpty() const { return (left == right || top == bottom); }
		int width() const { return right - left; }
		int height() const { return bottom - top; }
		bool pointIn(const Point& p) const { return pointIn(p.x, p.y); }
		bool pointIn(int x, int y) const { return x >= left && right >= x && y >= top && bottom >= y; }
		void moveY(int y) { int h = height(); top = y; bottom = top + h; }
		void moveX(int x) { int w = width(); left = x; right = left + w; }
		void moveXY(int x, int y) { moveX(x); moveY(y); }
		bool operator ==(const tagRect& other) const { return left == other.left && right == other.right && top == other.top && bottom == other.bottom; }
		bool operator !=(const tagRect& other) const { return !(*this == other); }
	}Rect;
	typedef struct tagColor {
		unsigned long color;
		tagColor() : tagColor(0, 0, 0, 0) {}
		tagColor(unsigned long val) : color(val) {}
		tagColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
			:color(((r | ((unsigned long)g << 8)) | (((unsigned long)b) << 16)) | (((unsigned long)a) << 24)) {}
		unsigned char r() const { return color & 0xFF; }
		unsigned char g() const { return (color >> 8) & 0xFF; }
		unsigned char b() const { return (color >> 16) & 0xFF; }
		unsigned char a() const { return (color >> 24) & 0xFF; }
		unsigned long rgb() const { return color & 0xFFFFFF; }
		unsigned long argb() const { return ((b() | ((unsigned long)g() << 8)) | (((unsigned long)r()) << 16)) | (((unsigned long)a()) << 24); }
		void setR(unsigned char r) { color = (color & 0xFFFFFF00) | ((unsigned long)r & 0xFF); }
		void setG(unsigned char g) { color = (color & 0xFFFF00FF) | (((unsigned long)g & 0xFF) << 8); }
		void setB(unsigned char b) { color = (color & 0xFF00FFFF) | (((unsigned long)b & 0xFF) << 16); }
		void setA(unsigned char a) { color = (color & 0x00FFFFFF) | (((unsigned long)a & 0xFF) << 24); }
		void set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { setA(a); setR(r); setG(g); setB(b); }
	}Color;
	}
}
#endif //RT_AnnotationBaseDefines_H_
