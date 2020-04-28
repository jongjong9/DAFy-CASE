/******************************************************************
* 파일 이름 : Line.cpp
* 기능 : 선의 추상화 클래스
* 작성자 : 송윤창
* 작성일자 : 2015년 3월 30일
*******************************************************************/

#include "Line.h"

Line::Line(float x, float y, float width, float height, QColor backGroundColor,
	Qt::PenStyle borderLine, QColor borderColor, String contents)
	: NShape(x, y, width, height, backGroundColor, borderLine, borderColor, contents) {
}

Line::~Line() {
}

Line::Line(const Line& source) :NShape(source) {
}

Line& Line::operator =(const Line& source) {
	NShape::operator=(source);
	return *this;
}