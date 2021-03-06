#include "DrawingTool.h"
#include "SelectingTool.h"

#include "../Interface/DrawingPaper.h"
#include "../Interface/FlowChartEditor.h"
#include "../Interface/Scroll.h"
#include "../Observer/ScrollController.h"
#include "../Shapes/Shape.h"
#include "../Shapes/Composite/FlowChart.h"
#include "../Paint/Zoom/Zoom.h"
#include "../Paint/Zoom/ZoomVisitor.h"
#include "../Paint/Coordinate/CoordinateConverter.h"
#include "../History/Registrar.h"

#include "../../Graphic Objects/Composite/Painter.h"

DrawingTool* DrawingTool::instance = 0;

DrawingTool::DrawingTool() {
}

DrawingTool* DrawingTool::Instance() {
	if (instance == 0) {
		instance = new DrawingTool;
	}
	return instance;
}

void DrawingTool::Destroy() {
	if (instance != 0) {
		delete instance;
		instance = 0;
	}
}

void DrawingTool::OnLButtonDown(DrawingPaper *canvas, QPointF point) {
	canvas->flowChart->UnSelectAll();

	Long positionX = 0;
	Long positionY = 0;
	if (canvas->scrollController != NULL) {
		positionX = canvas->scrollController->GetScroll(1)->value();
		positionY = canvas->scrollController->GetScroll(0)->value();
	}

	canvas->startX = point.x() + positionX;
	canvas->startY = point.y() + positionY;

	canvas->currentX = point.x() + positionX;
	canvas->currentY = point.y() + positionY;

	NShape *holdA4Paper = canvas->a4Paper->Clone();
	FlowChartVisitor *zoomVisitor = new ZoomVisitor(canvas->zoom);
	holdA4Paper->Accept(zoomVisitor);

	QPointF startReal(canvas->startX, canvas->startY);
	QPointF startVirtual = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertVirtual(startReal);

	startVirtual.setX(startVirtual.x() * 100 / canvas->zoom->GetRate());
	startVirtual.setY(startVirtual.y() * 100 / canvas->zoom->GetRate());

	startReal = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertReal(startVirtual);

	QPointF currentReal(canvas->currentX, canvas->currentY);
	QPointF currentVirtual = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertVirtual(currentReal);

	currentVirtual.setX(currentVirtual.x() * 100 / canvas->zoom->GetRate());
	currentVirtual.setY(currentVirtual.y() * 100 / canvas->zoom->GetRate());

	currentReal = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertReal(currentVirtual);

	canvas->startX = startReal.x();
	canvas->startY = startReal.y();

	canvas->currentX = currentReal.x();
	canvas->currentY = currentReal.y();

	canvas->templateSelected->Move(canvas->startX, canvas->startY);
	canvas->templateSelected->Select(true);
}

void DrawingTool::OnMouseMove(DrawingPaper *canvas, QPointF point) {
	Long positionX = 0;
	Long positionY = 0;
	if (canvas->scrollController != NULL) {
		positionX = canvas->scrollController->GetScroll(1)->value();
		positionY = canvas->scrollController->GetScroll(0)->value();
	}

	canvas->currentX = point.x() + positionX;
	canvas->currentY = point.y() + positionY;

	NShape *holdA4Paper = canvas->a4Paper->Clone();
	FlowChartVisitor *zoomVisitor = new ZoomVisitor(canvas->zoom);
	holdA4Paper->Accept(zoomVisitor);

	QPointF currentReal(canvas->currentX, canvas->currentY);
	QPointF currentVirtual = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertVirtual(currentReal);

	currentVirtual.setX(currentVirtual.x() * 100 / canvas->zoom->GetRate());
	currentVirtual.setY(currentVirtual.y() * 100 / canvas->zoom->GetRate());

	currentReal = dynamic_cast<ZoomVisitor*>(zoomVisitor)->converter->ConvertReal(currentVirtual);

	canvas->currentX = currentReal.x();
	canvas->currentY = currentReal.y();

	float width = canvas->currentX - canvas->startX;
	float height = canvas->currentY - canvas->startY;
	if (width >= 150.0F && height >= 50.0F) {
		canvas->templateSelected->ReSize(width, height);
	}

	canvas->repaint();
}

void DrawingTool::OnLButtonUp(DrawingPaper *canvas, QPointF point) {
	Long previousLength = canvas->flowChart->GetLength();

	canvas->indexOfSelected = canvas->flowChart->Attach(canvas->templateSelected->Clone());

	canvas->registrar->Register(canvas->flowChart->GetAt(canvas->indexOfSelected));

	if (canvas->templateSelected != NULL) {
		delete canvas->templateSelected;
		canvas->templateSelected = NULL;
	}
	canvas->tool = SelectingTool::Instance();
	canvas->mode = DrawingPaper::SELECT;
	canvas->repaint();
}