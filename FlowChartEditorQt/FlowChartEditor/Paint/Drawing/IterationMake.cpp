#include "IterationMake.h"
#include "../../Interface/DrawingPaper.h"
#include "../../Shapes/Shape.h"
#include "../../Shapes/Composite/FlowChart.h"
#include "../../Shapes/Symbols/Decision.h"
#include "../../Shapes/Symbols/Process.h"
#include "../../Shapes/Lines/Arrow.h"
#include "../../Shapes/Lines/RepeatTrue.h"
#include "../../Shapes/Lines/RepeatFalse.h"
#include "../../Shapes/Lines/RightDownJoin.h"
#include "../../Shapes/Lines/Join.h"
#include "../../History/Registrar.h"

IterationMake::IterationMake() {
}

IterationMake::~IterationMake() {
}

void IterationMake::Create(DrawingPaper *canvas) {
	// 1. 배열 첨자가 가장 작은 도형을 찾는다.
	Long(*indexes);
	Long length;
	NShape *shape;
	NShape *top, *repeatTrue;
	Long topIndex = -1;
	float x, y, width, height, width2, height2;
	Long i, j;
	Long index = -1;
	Long index_;
	Attribute attribute, initAttribute;
	QRectF rect;
	QRectF bufferRange;

	Long arrowIndex = -1;

	FlowChart buffer(canvas->flowChart->GetCapacity());

	canvas->flowChart->GetSelecteds(&indexes, &length);
	for (i = 0; i < length; i++) {
		shape = canvas->flowChart->GetAt(indexes[i]);
		buffer.Attach(shape->Clone());
		if (index > indexes[i] || index == -1) {
			index = indexes[i];
		}
	}
	if (indexes != 0) {
		delete[] indexes;
	}
	canvas->flowChart->DetachSelectedAll();

	i = 0;
	while (i < buffer.GetLength()) {
		if (dynamic_cast<Decision *>(buffer.GetAt(i))) {
			if (topIndex == -1 || buffer.GetAt(i)->GetY() < buffer.GetAt(topIndex)->GetY()) {
				topIndex = i;
			}
		}
		i++;
	}

	top = buffer.GetAt(topIndex)->Clone();
	buffer.Detach(topIndex);
	buffer.Insert(0, top);

	// top에 붙어 있는 선들을 모두 지운다.
	buffer.Find(ARROW | REPEATTRUE | REPEATFALSE, &indexes, &length);
	for (i = length - 1; i >= 0; i--) {
		attribute = initAttribute;
		if (dynamic_cast<RepeatTrue *>(buffer.GetAt(indexes[i]))) {
			buffer.GetAt(indexes[i])->GetAttribute(&attribute);
			NShape::MakeRectToPoint(attribute.pointOut, &rect);
		}
		else {
			buffer.GetAt(indexes[i])->GetAttribute(&attribute);
			NShape::MakeRectToPoint(attribute.pointIn, &rect);
		}
		if (top->IsIncluded(rect)) {
			if (!(dynamic_cast<Arrow*>(buffer.GetAt(indexes[i])))) {
				buffer.Detach(indexes[i]);
			}
			else {
				arrowIndex = indexes[i];
			}
		}
	}

	FlowChart temp(buffer);
	temp.AscendingSort();

	//top에 붙은 Arrow를 찾는다.

	//Arrow가 없으면 Arrow를 그린다.
	if (arrowIndex == -1) {
		attribute = initAttribute;
		top->GetAttribute(&attribute);
		x = attribute.pointOut.x();
		y = attribute.pointOut.y();
		width = 0;
		height = temp.GetAt(1)->GetY() - y;
		shape = new Arrow(x, y, width, height, QColor(0, 0, 0), Qt::SolidLine, QColor(0, 0, 0), String("TRUE"));
		canvas->registrar->Register(shape);
		shape->Select(true);
		Long position = buffer.Insert(1, shape);
	}
	else {
		//Arrow가 있으면 TRUE를 적는다.
		buffer.GetAt(arrowIndex)->Rewrite(String("TRUE"));
	}

	// 마지막이 Arrow이면 지운다.
	j = temp.GetLength() - 1;
	if (dynamic_cast<Arrow *>(temp.GetAt(j))) {
		index_ = buffer.Find(temp.GetAt(j));
		buffer.Detach(index_);
		temp.Detach(j);
		j--;
	}

	// RepeatTrue를 그린다.	
	attribute = initAttribute;
	temp.GetAt(j)->GetAttribute(&attribute);
	x = attribute.pointOut.x();
	y = attribute.pointOut.y();

	attribute = initAttribute;
	top->GetAttribute(&attribute);
	width = attribute.pointIn.x() - x - 10;
	height = attribute.pointIn.y() - y - 10;

	// 새로 만들 반복선의 범위를 구한다.
	bufferRange = buffer.GetRange();
	width2 = bufferRange.left() - 20 - x;
	height2 = y - bufferRange.bottom() + 40;
	repeatTrue = new RepeatTrue(x, y, width, height, width2, height2, QColor(0, 0, 0), Qt::SolidLine, QColor(0, 0, 0), String(" "));
	canvas->registrar->Register(repeatTrue);
	repeatTrue->Select(true);
	buffer.Attach(repeatTrue);

	attribute = initAttribute;
	top->GetAttribute(&attribute);
	x = attribute.pointFalse.x();
	y = attribute.pointFalse.y();
	width = repeatTrue->GetX() - x;
	width2 = bufferRange.right() - x + 20;
	height2 = repeatTrue->GetY() + dynamic_cast<RepeatTrue *>(repeatTrue)->GetHeight2() - y + 35;
	height = height2 + 40;

	shape = new RepeatFalse(x, y, width, height, width2, height2,
		QColor(0, 0, 0), Qt::SolidLine, QColor(0, 0, 0), String("FALSE"));
	canvas->registrar->Register(shape);
	shape->Select(true);
	buffer.Attach(shape);

	j = 0;
	for (i = 0; i < buffer.GetLength(); i++) {
		canvas->flowChart->Insert(index + i, buffer.GetAt(i)->Clone());
	}

	canvas->repaint();
}