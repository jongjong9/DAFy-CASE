/******************************************************************
 * 파일 이름 : FlowCahrtTemplate.cpp
 * 기능 : 순서도 작성자 컨트롤
 * 작성자 : 송윤창
 * 작성일자 : 2015년 4월 10일
*******************************************************************/

#include "FlowChartEditor.h"
#include "FlowChartTemplate.h"
#include "DrawingPaper.h"
#include "../Shapes/Shape.h"
#include "../Shapes/Composite/Template.h"
#include "../Shapes/Symbols/Symbol.h"
#include "../Shapes/Symbols/Terminal.h"
#include "../Shapes/Symbols/Decision.h"
#include "../Shapes/Symbols/Preparation.h"
#include "../Shapes/Symbols/Process.h"
#include "../Shapes/Symbols/InputOutput.h"
#include "../Shapes/Lines/Line.h"
#include "../Shapes/Lines/Arrow.h"
#include "../Shapes/Lines/Join.h"
#include "../Shapes/Lines/LeftDown.h"
#include "../Shapes/Lines/RepeatFalse.h"
#include "../Shapes/Lines/RepeatTrue.h"
#include "../Shapes/Lines/RightDown.h"
#include "../Shapes/Lines/RightDownJoin.h"
#include "../Shapes/Others/WindowTitle.h"
#include "../Shapes/Others/WindowPin.h"
#include "../Paint/Drawing/DrawVisitor.h"
#include "../KeyActions/FlowChartKeyActionFactory.h"
#include "../KeyActions/FlowChartKeyActions.h"

#include "../../Utilities/Array.h"
#include "../../Graphic Objects/Composite/Painter.h"
#include "../../Graphic Objects/Composite/Qt/QtPainter.h"
#include "../../Graphic Objects/Factory/Qt/QtGObjectFactory.h"

#include <qpainter.h>
#include <qevent.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qstatusbar.h>

#include <Windows.h>

FlowChartTemplate::FlowChartTemplate(QWidget *parent)
	: QFrame(parent) {
	this->setMouseTracking(true);
	this->setFocusPolicy(Qt::StrongFocus);

	this->shapeSelected = NULL;
	this->mode = DRAWOFF;
	this->oldShapeSelected = NULL;
	this->windowBorderColor = QColor(255, 255, 255);

	QRect rect = this->frameRect();
	float width = 150.0F;
	float height = 50.0F;
	float x = (190.0F - width) / 2.0F;
	float y = 50.0F;

	this->flowChartTemplate = new Template;
	NShape *template1 = new Terminal(x, y, width, height, QColor(255, 153, 153),
		Qt::SolidLine, QColor(0, 0, 0), String("START"));
	y += 70.0F;
	NShape *template2 = new Preparation(x, y, width, height, QColor(153, 153, 255),
		Qt::SolidLine, QColor(0, 0, 0), String("Preparation"));
	y += 70.0F;
	NShape *template3 = new InputOutput(x, y, width, height, QColor(255, 255, 153),
		Qt::SolidLine, QColor(0, 0, 0), String("READ "));
	y += 70.0F;
	NShape *template4 = new Process(x, y, width, height, QColor(153, 255, 153),
		Qt::SolidLine, QColor(0, 0, 0), String("Process"));
	y += 70.0F;
	NShape *template5 = new Decision(x, y, width, height, QColor(255, 153, 255),
		Qt::SolidLine, QColor(0, 0, 0), String("Decision"));
	y += 70.0F;
	NShape *template6 = new InputOutput(x, y, width, height, QColor(255, 255, 153),
		Qt::SolidLine, QColor(0, 0, 0), String("PRINT "));
	y += 70.0F;
	NShape *template7 = new Terminal(x, y, width, height, QColor(255, 153, 153),
		Qt::SolidLine, QColor(0, 0, 0), String("STOP"));

	this->flowChartTemplate->Attach(template1);
	this->flowChartTemplate->Attach(template2);
	this->flowChartTemplate->Attach(template3);
	this->flowChartTemplate->Attach(template4);
	this->flowChartTemplate->Attach(template5);
	this->flowChartTemplate->Attach(template6);
	this->flowChartTemplate->Attach(template7);

	this->windowTitle = new WindowTitle(2.0F, 2.0F, 186.0F, 30.0F, QColor(235, 235, 235),
		Qt::SolidLine, QColor(255, 255, 255), String(" 기호 상자")); //x, y는 창 테두리 두께 5와 타이틀 두께 1의 기시감? 해결
	float windowPinX = this->windowTitle->GetX() + this->windowTitle->GetWidth() - 26 - 3; //24=사각형길이,3=여유공간
	float windowPinY = this->windowTitle->GetY() + 4;
	this->windowPin = new WindowPin(windowPinX, windowPinY, 26, 23, QColor(235, 235, 235),
		Qt::SolidLine, QColor(255, 255, 255));

	this->painter = new QtPainter((Long)rect.width(), (Long)rect.height());

	DrawingPaper *canvas = static_cast<DrawingPaper*>(static_cast<FlowChartEditor*>(this->parentWidget())->windows[0]);
	GObject *font = canvas->painter->CurrentObject("Font");
	this->painter->SelectObject(*font->Clone());
	this->painter->Update();
}

FlowChartTemplate::~FlowChartTemplate() {
	if (this->flowChartTemplate != NULL) {
		delete this->flowChartTemplate;
	}
	if (this->painter != NULL) {
		delete this->painter;
	}
	if (this->windowTitle != NULL) {
		delete this->windowTitle;
	}
	if (this->windowPin != NULL) {
		delete this->windowPin;
	}
}

void FlowChartTemplate::paintEvent(QPaintEvent *event) {
	QPainter dc(this);

	int oldMode = this->painter->GetBackgroundMode();
	this->painter->SetBackgroundMode(Qt::TransparentMode);
	this->painter->SetCompositionMode(QPainter::RasterOp_NotSourceXorDestination);

	QtGObjectFactory factory;

	QRect frameRect = this->frameRect();

	this->painter->Resize(frameRect.width(), frameRect.height()); // canvas size 변경

	//=======창 테두리=========
	GObject *pen = factory.MakePen(QBrush(this->windowBorderColor), 5);
	GObject *oldPen = this->painter->SelectObject(*pen);
	this->painter->Update();

	this->painter->DrawRect(QRect(0, 0, 190, frameRect.height()));

	this->painter->SelectObject(*oldPen);
	this->painter->Update();
	if (pen != 0) {
		delete pen;
	}
	//=======창 테두리=========

	FlowChartVisitor *visitor = new DrawVisitor(this->painter);
	this->flowChartTemplate->Accept(visitor);

	//창 제목 먼저
	this->windowTitle->Accept(visitor);
	//고정 핀
	this->windowPin->Accept(visitor);

	//======================
	this->painter->Render(&dc, 0, 0);

	if (visitor != 0) {
		delete visitor;
	}
}

void FlowChartTemplate::mousePressEvent(QMouseEvent *event) {
	FlowChartEditor* editor;

	editor = static_cast<FlowChartEditor*>(this->parentWidget());

	Long index = -1;

	index = this->flowChartTemplate->Find(event->localPos());
	if (index != -1) {

		this->shapeSelected = this->flowChartTemplate->GetAt(index);

		static_cast<DrawingPaper*>(editor->windows[0])->mode = DrawingPaper::DRAWING;

		editor->modeStatus->setText(QString::fromLocal8Bit("DRAWING"));
		QString style;
		switch (this->shapeSelected->GetSymbolID()) {
		case ID_TERMINAL:
			style = QString::fromLocal8Bit("    단말 기호"); break;
		case ID_PREPARATION:
			style = QString::fromLocal8Bit("    준비 기호"); break;
		case ID_INPUTOUTPUT:
			style = QString::fromLocal8Bit("    입출력 기호"); break;
		case ID_PROCESS:
			style = QString::fromLocal8Bit("    처리 기호"); break;
		case ID_DECISION:
			style = QString::fromLocal8Bit("    판단 기호"); break;
		}
		editor->messageStatus->setText(style);
		editor->statusBar->repaint();
	}
	else {
		this->shapeSelected = NULL;
	}

	this->repaint();
}

void FlowChartTemplate::mouseMoveEvent(QMouseEvent *event) {
	FlowChartEditor *editor = static_cast<FlowChartEditor*>(this->parentWidget());
	editor->windowClose->Paint(QColor(102, 204, 204), Qt::SolidLine, editor->windowClose->GetBorderColor());
	editor->repaint();

	//기호들
	Long index = -1;

	QColor selectedColor(235, 235, 235);
	NShape *shape;
	Long i = 0;
	while (i < 7/*this->flowChartTemplate->GetLength()*/) {
		shape = this->flowChartTemplate->GetAt(i);
		if (shape->GetBackGroundColor() == selectedColor && this->oldShapeSelected != NULL) {
			shape->Paint(this->oldShapeSelected->GetBackGroundColor(), shape->GetBorderLine(), shape->GetBorderColor());
		}
		i++;
	}
	index = this->flowChartTemplate->Find(event->localPos());

	if (index != -1) {
		shape = this->flowChartTemplate->GetAt(index);
		this->oldShapeSelected = shape->Clone();
		shape->Paint(selectedColor, shape->GetBorderLine(), shape->GetBorderColor());
	}

	//윈도우 핀
	QRectF pinRect(this->windowPin->GetX(), this->windowPin->GetY(), this->windowPin->GetWidth(), this->windowPin->GetHeight());
	bool isContain = pinRect.contains(event->localPos());
	if (isContain == true && this->hasFocus()) {
		this->windowPin->Paint(QColor(102, 255, 255), Qt::SolidLine, this->windowPin->GetBorderColor());
	}
	else if (isContain == false && this->hasFocus()) {
		this->windowPin->Paint(QColor(102, 204, 204), Qt::SolidLine, this->windowPin->GetBorderColor());
	}
	else if (isContain == true && !this->hasFocus()) {
		this->windowPin->Paint(QColor(204, 204, 204), Qt::SolidLine, this->windowPin->GetBorderColor());
	}
	else { //isContain == false && !this->hasFocus()
		this->windowPin->Paint(QColor(235, 235, 235), Qt::SolidLine, this->windowPin->GetBorderColor());
	}

	//고정 해제된 기호 상자 윈도우 타이틀에 마우스 올렸을 때
	bool isFocusedAndPinned = dynamic_cast<WindowTitle*>(this->windowTitle)->GetIsFocusedAndPinned();
	if (isFocusedAndPinned == false) {
		QRectF titleRect(this->x(), this->y(), this->width(), this->height());
		isContain = titleRect.contains(event->localPos());
		if (isContain == true && this->hasFocus()) {
			this->windowTitle->Paint(QColor(102, 204, 204), this->windowTitle->GetBorderLine(), QColor(102, 204, 204));
			this->windowBorderColor = this->windowTitle->GetBorderColor();
			this->windowPin->Paint(QColor(102, 204, 204), Qt::SolidLine, this->windowPin->GetBorderColor());
		}
	}

	this->repaint();
}

void FlowChartTemplate::mouseReleaseEvent(QMouseEvent *event) {
	QRectF pinRect(this->windowPin->GetX(), this->windowPin->GetY(), this->windowPin->GetWidth(), this->windowPin->GetHeight());
	bool isContain = pinRect.contains(event->localPos());
	bool isPinned = dynamic_cast<WindowPin*>(this->windowPin)->GetIsPinned();
	bool previousIsPinned = isPinned;
	if (isContain == true) {
		if (isPinned == true) {
			dynamic_cast<WindowPin*>(this->windowPin)->SetIsPinned(false);
		}
		else {
			dynamic_cast<WindowPin*>(this->windowPin)->SetIsPinned(true);
		}
	}

	isPinned = dynamic_cast<WindowPin*>(this->windowPin)->GetIsPinned();
	if (previousIsPinned == false && isPinned == true) {
		FlowChartEditor *editor = (FlowChartEditor*)this->parentWidget();
		DrawingPaper *canvas = (DrawingPaper*)editor->windows[0];
		Long newX = this->x() * 2 + this->width();
		Long newWidth = editor->width() - newX;
		canvas->move(newX, canvas->y());
		canvas->resize(newWidth, canvas->height());
	}
	else if (previousIsPinned == true && isPinned == false) {
		this->clearFocus();
	}
}

void FlowChartTemplate::focusOutEvent(QFocusEvent *event) {
	this->windowTitle->Paint(QColor(235, 235, 235), this->windowTitle->GetBorderLine(), QColor(255, 255, 255));
	this->windowBorderColor = this->windowTitle->GetBorderColor();
	this->windowPin->Paint(QColor(235, 235, 235), Qt::SolidLine, this->windowPin->GetBorderColor());

	bool isPinned = dynamic_cast<WindowPin*>(this->windowPin)->GetIsPinned();
	if (isPinned == false) { //고정 해제된 상태에서 포커스 아웃됨.
		
		QString verticalContent = QString::fromLocal8Bit(this->windowTitle->GetContents());
		verticalContent.remove(QChar(' '));
		Long i = 1;
		while (i < verticalContent.size()) {
			verticalContent.insert(i, '\n');
			i += 2;
		}
		QRectF rectangle(0, 0, 500, 500);
		QRectF rect = this->painter->BoundingRect(rectangle, Qt::AlignCenter, verticalContent);

		this->windowTitle->ReSize(this->windowTitle->GetHeight(), rect.height());
		this->windowTitle->Move(this->windowTitle->GetX(), this->windowTitle->GetY() - 3);
		this->resize(this->windowTitle->GetWidth(), rect.height() - 3);

		dynamic_cast<WindowTitle*>(this->windowTitle)->SetIsFocusedAndPinned(false);

		this->windowTitle->Paint(QColor(235, 235, 235), this->windowTitle->GetBorderLine(), QColor(255, 255, 255));
		this->windowBorderColor = this->windowTitle->GetBorderColor();

		//DrawingPaper넹?
		FlowChartEditor *editor = (FlowChartEditor*)this->parentWidget();
		DrawingPaper *canvas = (DrawingPaper*)editor->windows[0];
		Long newX = this->x() * 2 + this->windowTitle->GetWidth();
		Long newWidth = editor->width() - newX;
		canvas->move(newX, canvas->y());
		canvas->resize(newWidth, canvas->height());
	}
	this->repaint();
}

void FlowChartTemplate::focusInEvent(QFocusEvent *event) {
	dynamic_cast<WindowTitle*>(this->windowTitle)->SetIsFocusedAndPinned(true);
	this->windowTitle->Paint(QColor(102, 204, 204), this->windowTitle->GetBorderLine(), QColor(102, 204, 204));
	this->windowBorderColor = this->windowTitle->GetBorderColor();
	this->windowPin->Paint(QColor(102, 204, 204), Qt::SolidLine, this->windowPin->GetBorderColor());

	FlowChartEditor *editor = (FlowChartEditor*)this->parentWidget();
	bool isPinned = dynamic_cast<WindowPin*>(this->windowPin)->GetIsPinned();
	if (isPinned == false) {
		this->resize(190, editor->frameRect().height() - editor->menuBar->height() - editor->statusBar->height() - 20);
		this->windowTitle->ReSize(186, 30);
		this->windowTitle->Move(this->windowTitle->GetX(), this->windowTitle->GetY() + 3);
	}
	else {
		//DrawingPaper
		DrawingPaper *canvas = (DrawingPaper*)editor->windows[0];
		Long newX = this->x() * 2 + this->width();
		Long newWidth = editor->width() - newX;
		canvas->move(newX, canvas->y());
		canvas->resize(newWidth, canvas->height());
	}
	this->repaint();
}

void FlowChartTemplate::leaveEvent(QEvent *event) {
	//고정해제, 포커스 아웃, 마우스 올리고 치울 때 색깔 처리
	if (!dynamic_cast<WindowTitle*>(this->windowTitle)->GetIsFocusedAndPinned()) {
		this->windowTitle->Paint(QColor(235, 235, 235), this->windowTitle->GetBorderLine(), QColor(255, 255, 255));
		this->windowBorderColor = this->windowTitle->GetBorderColor();
	}

	//기호들 바뀐 색깔 유지되는거 방지
	QColor selectedColor(235, 235, 235);
	NShape *shape;
	Long i = 0;
	while (i < 7/*this->flowChartTemplate->GetLength()*/) {
		shape = this->flowChartTemplate->GetAt(i);
		if (shape->GetBackGroundColor() == selectedColor && this->oldShapeSelected != NULL) {
			shape->Paint(this->oldShapeSelected->GetBackGroundColor(), shape->GetBorderLine(), shape->GetBorderColor());
		}
		i++;
	}

	//윈도우 핀
	if (this->hasFocus()) {
		this->windowPin->Paint(QColor(102, 204, 204), Qt::SolidLine, this->windowPin->GetBorderColor());
	}
	else {
		this->windowPin->Paint(QColor(235, 235, 235), Qt::SolidLine, this->windowPin->GetBorderColor());
	}

	this->repaint();
}

void FlowChartTemplate::enterEvent(QEvent *event) {
	//고정해제, 포커스 아웃, 마우스 올리고 치울 때 색깔 처리
	if (!dynamic_cast<WindowTitle*>(this->windowTitle)->GetIsFocusedAndPinned()) {
		this->windowTitle->Paint(QColor(235, 235, 235), this->windowTitle->GetBorderLine(), QColor(102, 204, 204));
		this->windowBorderColor = this->windowTitle->GetBorderColor();
	}

	this->repaint();
}

void FlowChartTemplate::keyPressEvent(QKeyEvent *event) {
	FlowChartEditor *editor = (FlowChartEditor*)this->parentWidget();
	FlowChartKeyActionFactory keyActionFactory(editor);
	FlowChartKeyAction *keyAction = keyActionFactory.Make(event->modifiers(), event->key());
	if (keyAction != 0) {
		keyAction->OnKeyDown();
		delete keyAction;

		QString mode = static_cast<DrawingPaper*>(editor->windows[0])->GetCurrentMode();
		editor->modeStatus->setText(mode);
		editor->statusBar->repaint();
	}
	editor->windows[1]->repaint(); //왜?

	this->repaint();
}