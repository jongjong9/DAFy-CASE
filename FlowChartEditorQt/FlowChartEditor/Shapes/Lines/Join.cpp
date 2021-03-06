/******************************************************************
* 파일 이름 : Join.cpp
* 기능 : 연결선 콘크리트 클래스
* 작성자 : 송윤창
* 작성일자 : 2015년 3월 31일
*******************************************************************/
#pragma warning (disable : 4996)

#include "Join.h"
#include "../../Paint/FlowChartVisitor.h"

#include "../../Interface/Scroll.h"
#include "../../Observer/ScrollController.h"

#include "../../../Graphic Objects/Composite/Painter.h"
#include "../../../Graphic Objects/Factory/Qt/QtGObjectFactory.h"

Join::Join(float x, float y, float width, float height, float height2, QColor backGroundColor,
	Qt::PenStyle borderLine, QColor borderColor, String contents)
	: Line(x, y, width, height, backGroundColor, borderLine, borderColor, contents) {
	this->height2 = height2;
}

Join::~Join() {

}

Join::Join(const Join& source)
	: Line(source) {
	this->height2 = source.height2;
}

Join& Join::operator =(const Join& source) {
	Line::operator=(source);
	this->height2 = source.height2;

	return *this;
}

bool Join::IsEqual(const NShape& other) {
	bool retVo = false;
	if (dynamic_cast<Join *>(const_cast<NShape *>(&other)) &&
		dynamic_cast<Join *>(const_cast<NShape *>(&other))->height2 == this->height2) {
		retVo = NShape::IsEqual(other);
	}
	return retVo;
}

bool Join::IsNotEqual(const NShape& other) {
	bool retVo = false;
	if (!dynamic_cast<Join *>(const_cast<NShape *>(&other)) ||
		dynamic_cast<Join *>(const_cast<NShape *>(&other))->height2 != this->height2) {
		retVo = NShape::IsNotEqual(other);
	}
	return retVo;
}

bool Join::operator ==(const NShape& other) {
	bool retVo = false;
	if (dynamic_cast<Join *>(const_cast<NShape *>(&other))) {
		retVo = NShape::IsEqual(other);
	}
	return retVo;
}

bool Join::operator !=(const NShape& other) {
	bool retVo = false;
	if (!dynamic_cast<Join *>(const_cast<NShape*>(&other))) {
		retVo = NShape::IsNotEqual(other);
	}
	return retVo;
}

NShape* Join::Clone() {
	return new Join(*this);
}

void Join::Accept(FlowChartVisitor *draw) {
	draw->Visit(this);
}

void Join::DrawActiveShape(GObject *painter) {
	QPointF points[4];
	points[0] = QPoint(this->x, this->y);
	points[1] = QPoint(this->x, this->y + this->height2);
	points[2] = QPoint(this->x + this->width, this->y + this->height2);
	points[3] = QPoint(this->x + this->width, this->y + this->height);

	painter->DrawPolyline(points, 4);
}

QRegion Join::GetRegion() {
	QRectF rect(this->x - LINETHICKNESS, this->y, LINETHICKNESS * 2, this->height2);
	QRegion region(rect.toRect());

	rect = QRectF(this->x, this->y + this->height2 - LINETHICKNESS,
		this->width, LINETHICKNESS * 2);
	region += QRegion(rect.toRect());

	rect = QRectF(this->x + this->width - LINETHICKNESS, this->y + this->height,
		LINETHICKNESS*2, 
		(this->height > 0) ? (this->height2 - this->height) : (this->height * -1 + this->height2));
	region += QRegion(rect.toRect());

	return region;
}

QRegion Join::GetRegion(Long thickness) {
	QRectF rect(this->x - thickness, this->y, thickness*2, this->height2);
	QRegion region(rect.toRect());

	rect = QRectF(this->x, this->y + this->height2 - thickness,
		this->width, thickness*2);
	region += QRegion(rect.toRect());

	rect = QRectF(this->x + this->width - thickness, this->y + this->height,
		thickness*2,
		(this->height > 0) ? (this->height2 - this->height) : (this->height * -1 + this->height2));
	region += QRegion(rect.toRect());

	return region;
}

void Join::ReSize(float width, float height, float height2) {
	this->width = width;
	this->height = height;
	this->height2 = height2;
}

bool Join::IsIncluded(QPointF point) {
	bool ret;

	QRectF regionRect(this->x - LINETHICKNESS, this->y,
		LINETHICKNESS*2, this->height2);
	QRegion region(regionRect.toRect());

	regionRect = QRectF(this->x, this->y + this->height2 - LINETHICKNESS,
		this->width, LINETHICKNESS*2);
	region += QRegion(regionRect.toRect());

	regionRect = QRectF(this->x + this->width - LINETHICKNESS, this->y + this->height,
		LINETHICKNESS*2, 
		(this->height > 0) ? (this->height2 - this->height) : (this->height * -1 + this->height2));
	region += QRegion(regionRect.toRect());

	ret = region.contains(point.toPoint());

	return ret;
}

bool Join::IsIncluded(const QRectF& rect) {
	bool ret;

	QRectF regionRect(this->x - LINETHICKNESS, this->y,
		LINETHICKNESS * 2, this->height2);
	QRegion region(regionRect.toRect());

	regionRect = QRectF(this->x, this->y + this->height2 - LINETHICKNESS,
		this->width, LINETHICKNESS * 2);
	region += QRegion(regionRect.toRect());

	regionRect = QRectF(this->x + this->width - LINETHICKNESS, this->y + this->height,
		LINETHICKNESS * 2, 
		(this->height > 0) ? (this->height2-this->height) : (this->height * -1 + this->height2));
	region += QRegion(regionRect.toRect());

	ret = region.contains(rect.toRect());

	return ret;
}

void Join::Copy(NShape *object) {
	this->x = object->GetX();
	this->y = object->GetY();
	this->width = object->GetWidth();
	this->height = object->GetHeight();
	this->backGroundColor = object->GetBackGroundColor();
	this->borderLine = object->GetBorderLine();
	this->borderColor = object->GetBorderColor();
	this->contents = object->GetContents();
	this->height2 = dynamic_cast<Join *>(object)->GetHeight2();
	this->isSelected = object->IsSelected();
}

void Join::GetSelectionMarkerRect(int marker, QRectF *rect) {
	float x;
	float y;

	switch (marker) {
	case HIT_TRUE:
		x = this->x;
		y = this->y;
		break;

	case HIT_OUT:
		x = this->x + this->width / 2;
		y = this->y + this->height2;
		break;

	case HIT_FALSE:
		x = this->x + this->width;
		y = this->y + this->height;
		break;
	}

	rect->setCoords(x - 4, y - 4, x + 5, y + 5);
}

QRegion Join::GetSelectionMarkerAllRegion() {
	float x = this->x;
	float y = this->y;
	QRectF rect(x - 5, y - 5, 6, 6);
	QRegion region(rect.toRect());

	x = this->x + this->width / 2;
	y = this->y + this->height2;
	rect = QRectF(x - 5, y - 5, 6, 6);
	region += QRegion(rect.toRect());

	x = this->x + this->width;
	y = this->y + this->height;
	rect = QRectF(x - 5, y - 5, 6, 6);
	region += QRegion(rect.toRect());

	return region;
}

int Join::GetHitCode(const QPointF& point, const QRegion& region) {
	int result = HIT_NONE;

	if (region.contains(point.toPoint())) {
		result = HIT_BODY;
	}

	QRectF rectSelect;
	this->GetSelectionMarkerRect(HIT_TRUE, &rectSelect);
	if (rectSelect.contains(point)) {
		result = HIT_TRUE;
	}

	this->GetSelectionMarkerRect(HIT_OUT, &rectSelect);
	if (rectSelect.contains(point)) {
		result = HIT_OUT;
	}

	this->GetSelectionMarkerRect(HIT_FALSE, &rectSelect);
	if (rectSelect.contains(point)) {
		result = HIT_FALSE;
	}

	return result;
}

void Join::DrawSelectionMarkers(GObject *painter, ScrollController *scrollController) {
	QRectF rectSelect;

	QtGObjectFactory factory;
	GObject *brush = factory.MakeBrush(QColor(0, 0, 255), Qt::SolidPattern);
	GObject *oldBrush = painter->SelectObject(*brush);
	painter->Update();

	GetSelectionMarkerRect(HIT_TRUE, &rectSelect);
	Long positionX = scrollController->GetScroll(1)->value();
	Long positionY = scrollController->GetScroll(0)->value();
	rectSelect.setCoords(rectSelect.left() - positionX, rectSelect.top() - positionY,
		rectSelect.right() - positionX, rectSelect.bottom() - positionY);
	painter->DrawRect(rectSelect);

	GetSelectionMarkerRect(HIT_OUT, &rectSelect);
	rectSelect.setCoords(rectSelect.left() - positionX, rectSelect.top() - positionY,
		rectSelect.right() - positionX, rectSelect.bottom() - positionY);
	painter->DrawRect(rectSelect);

	GetSelectionMarkerRect(HIT_FALSE, &rectSelect);
	rectSelect.setCoords(rectSelect.left() - positionX, rectSelect.top() - positionY,
		rectSelect.right() - positionX, rectSelect.bottom() - positionY);
	painter->DrawRect(rectSelect);

	painter->SelectObject(*oldBrush);
	painter->Update();
	if (brush != NULL) {
		delete brush;
	}
}

void Join::GetAttribute(Attribute *attribute) {
	attribute->vertexOut = 'Y';
	attribute->vertexTrue = 'Y';
	attribute->vertexFalse = 'Y';

	attribute->pointOut.setX(this->x + this->width / 2);
	attribute->pointOut.setY(this->y + this->height2);

	attribute->pointIn = attribute->pointOut;

	attribute->pointTrue.setX(this->x);
	attribute->pointTrue.setY(this->y);

	attribute->pointFalse.setX(this->x + this->width);
	attribute->pointFalse.setY(this->y + this->height);
}

void Join::GetLine(char(*line)) {
	sprintf(line, "%d\t%f\t%f\t%f\t%f\t\t%f\t%s;\n",
		ID_JOIN, this->x, this->y, this->width, this->height, this->height2, this->contents.GetString());
}


bool Join::IsStyle(Long style) {
	bool ret = false;
	if (NShape::IsStyle(style) || Line::IsStyle(style) || (style >> 12) % 2) {
		ret = true;
	}
	return ret;
}