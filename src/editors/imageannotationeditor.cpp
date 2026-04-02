#include "include/editors/imageannotationeditor.h"
#include "ui_imageannotationeditor.h"

ImageAnnotationEditor::ImageAnnotationEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageAnnotationEditor)
{
    ui->setupUi(this);

    //These toolbuttons are here because they have special functionality (being colored in for example) so I cant place them in the editor
    QToolButton *lineColorButton = new QToolButton(this);
    lineColorButton->setText("Line Color");
    lineColorButton->setToolTip("Pick line color");
    QPixmap px1(16, 16);
    px1.fill(currentLineColor);
    lineColorButton->setIcon(QIcon(px1));
    ui->EditShapesToolBar->addWidget(lineColorButton);
    connect(lineColorButton, &QToolButton::clicked, this, [this, lineColorButton]() {
        QColor color = QColorDialog::getColor(currentLineColor, this, "Pick Line Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            currentLineColor = color;
            QPixmap px(16, 16);
            px.fill(color);
            lineColorButton->setIcon(QIcon(px));
        }
    });

    QToolButton *fillColorButton = new QToolButton(this);
    fillColorButton->setText("Fill Color");
    fillColorButton->setToolTip("Pick fill color");
    QPixmap px2(16, 16);
    px2.fill(currentFillColor);
    fillColorButton->setIcon(QIcon(px2));
    ui->EditShapesToolBar->addWidget(fillColorButton);
    connect(fillColorButton, &QToolButton::clicked, this, [this, fillColorButton]() {
        QColor color = QColorDialog::getColor(currentFillColor, this, "Pick Fill Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            currentFillColor = color;
            QPixmap px(16, 16);
            px.fill(color);
            fillColorButton->setIcon(QIcon(px));
        }
    });

    QComboBox *lineStyleCombo = new QComboBox(this);
    lineStyleCombo->addItem("Solid", static_cast<int>(Qt::SolidLine));
    lineStyleCombo->addItem("Dash", static_cast<int>(Qt::DashLine));
    lineStyleCombo->addItem("Dot", static_cast<int>(Qt::DotLine));
    lineStyleCombo->addItem("Dash-Dot", static_cast<int>(Qt::DashDotLine));
    lineStyleCombo->addItem("Dash-Dot-Dot", static_cast<int>(Qt::DashDotDotLine));
    lineStyleCombo->setToolTip("Line style");
    ui->EditShapesToolBar->addWidget(lineStyleCombo);
    connect(lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, lineStyleCombo](int index) {
        currentPenStyle = static_cast<Qt::PenStyle>(lineStyleCombo->itemData(index).toInt());
    });

    QComboBox *fillStyleCombo = new QComboBox(this);
    fillStyleCombo->addItem("Solid", static_cast<int>(Qt::SolidPattern));
    fillStyleCombo->addItem("None", static_cast<int>(Qt::NoBrush));
    fillStyleCombo->addItem("Dense", static_cast<int>(Qt::Dense4Pattern));
    fillStyleCombo->addItem("Horizontal", static_cast<int>(Qt::HorPattern));
    fillStyleCombo->addItem("Vertical", static_cast<int>(Qt::VerPattern));
    fillStyleCombo->addItem("Cross", static_cast<int>(Qt::CrossPattern));
    fillStyleCombo->addItem("Diagonal", static_cast<int>(Qt::BDiagPattern));
    fillStyleCombo->setToolTip("Fill style");
    ui->EditShapesToolBar->addWidget(fillStyleCombo);
    connect(fillStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, fillStyleCombo](int index) {
        currentBrushStyle = static_cast<Qt::BrushStyle>(fillStyleCombo->itemData(index).toInt());
    });

    QCheckBox *roundingCheck = new QCheckBox("Round", this);
    roundingCheck->setToolTip("Enable rounded corners");
    ui->EditShapesToolBar->addWidget(roundingCheck);

    QSpinBox *roundingSpin = new QSpinBox(this);
    roundingSpin->setRange(0, 100);
    roundingSpin->setValue(0);
    roundingSpin->setSuffix("px");
    roundingSpin->setToolTip("Rounding radius");
    roundingSpin->setEnabled(false);
    ui->EditShapesToolBar->addWidget(roundingSpin);

    connect(roundingCheck, &QCheckBox::toggled, this, [this, roundingSpin](bool checked) {
        roundingSpin->setEnabled(checked);
        currentRounding = checked ? roundingSpin->value() : 0;
    });
    connect(roundingSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        currentRounding = value;
    });
}

ImageAnnotationEditor::~ImageAnnotationEditor()
{
    delete ui;
}

void ImageAnnotationEditor::Initialize(ImageAnnotationData *data)
{
    myData = data;
    QPixmap *pixMap = new QPixmap();
    pixMap->loadFromData(myData->imageData);
    imageItem = new QGraphicsPixmapItem();
    imageItem->setPixmap(*pixMap);
    imageItem->setZValue(0);
    imageItem->setEnabled(false);
    graphicsView = new CustomGraphicsView(ui->centralwidget);
    ui->centralwidget->layout()->addWidget(graphicsView);
    graphicsView->scene()->addItem(imageItem);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    UpdateMarkers();
    UpdateShapes();
    ui->EditMarkersToolBar->hide();
    ui->EditShapesToolBar->hide();
}

void ImageAnnotationEditor::UpdateMarkers()
{
    for(int i = 0; i < myMarkers->count(); i++)
    {
        (*myMarkers)[i]->deleteLater();
    }
    myMarkers->clear();
    for(int i = 0; i < myData->markers.count(); i++)
    {
        MarkerData *dat = &myData->markers[i];
        MarkerItem *mark = new MarkerItem(dat);
        mark->setPos(dat->X * imageItem->pixmap().width(), dat->Y * imageItem->pixmap().height());
        mark->setZValue(1);
        graphicsView->scene()->addItem(mark);
        myMarkers->append(mark);

        connect(mark, &MarkerItem::clicked, this, &ImageAnnotationEditor::ClickedMarker);
        connect(mark, &MarkerItem::rightClicked, this, &ImageAnnotationEditor::RightClickedMarker);
        connect(mark, &MarkerItem::dragged, this, &ImageAnnotationEditor::dragMarkers);
        connect(mark, &MarkerItem::draggingFinished, this, &ImageAnnotationEditor::finishDragging);
        connect(mark, &MarkerItem::preClick, this, [this]() {
            graphicsView->panningLocked = true;
        });
        connect(mark, &MarkerItem::interactionFinished, this, [this]() {
            graphicsView->panningLocked = false;
        });
    }
}

void ImageAnnotationEditor::UpdateShapes()
{
    for(int i = 0; i < myShapes->count(); i++)
    {
        (*myShapes)[i]->deleteLater();
    }
    myShapes->clear();
    for(int i = 0; i < myData->shapes.count(); i++)
    {
        ShapeData *dat = &myData->shapes[i];
        ShapeGraphicsObject *obj = new ShapeGraphicsObject(dat, imageItem, &undoStack); //Stops here
        graphicsView->scene()->addItem(obj);
        myShapes->append(obj);
        connect(obj, &ShapeGraphicsObject::rightClicked, this, &ImageAnnotationEditor::RightClickedShape);
    }
}

MarkerData ImageAnnotationEditor::CreateMarker(double x, double y, MarkerData *pregenData)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create New Marker");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *labelEdit = new QLineEdit(&dialog);
    if(pregenData != nullptr)
        labelEdit->setText(pregenData->Label);
    layout->addWidget(new QLabel("Label:", &dialog));
    layout->addWidget(labelEdit);

    QColor chosenColor("#FFFFFF");
    if(pregenData != nullptr)
        chosenColor = pregenData->Color;
    QPushButton *colorPick = new QPushButton(&dialog);
    colorPick->setStyleSheet("background-color: " + chosenColor.name() + ";");
    colorPick->setFixedHeight(24);
    connect(colorPick, &QPushButton::clicked, this, [&chosenColor, colorPick, &dialog]() {
        QColor c = QColorDialog::getColor(chosenColor, &dialog, "Pick a color");
        if (c.isValid()) {
            chosenColor = c;
            colorPick->setStyleSheet("background-color: " + c.name() + ";");
        }
    });
    layout->addWidget(new QLabel("Color:", &dialog));
    layout->addWidget(colorPick);

    QLineEdit *sizeEdit = new QLineEdit(&dialog);
    sizeEdit->setValidator(new QIntValidator(0, 999, this));
    if(pregenData != nullptr)
        sizeEdit->setText(QString::number( pregenData->size ));
    else
        sizeEdit->setText("16");
    layout->addWidget(new QLabel("Size:", &dialog));
    layout->addWidget(sizeEdit);

    QStringList iconIds = { "Circle", "D8" };
    QString chosenIconId = iconIds.first();
    if(pregenData != nullptr)
        chosenIconId = pregenData->IconId;
    QHBoxLayout *iconLayout = new QHBoxLayout();
    QButtonGroup *iconGroup = new QButtonGroup(&dialog);
    iconGroup->setExclusive(true);
    for (const QString &iconId : iconIds)
    {
        QPushButton *iconBtn = new QPushButton(&dialog);
        iconBtn->setCheckable(true);
        iconBtn->setFixedSize(36, 36);
        iconBtn->setIcon(QIcon(":/icons/markers/" + iconId + ".svg"));
        iconBtn->setIconSize(QSize(24, 24));
        if (iconId == chosenIconId)
            iconBtn->setChecked(true);
        iconGroup->addButton(iconBtn);
        iconLayout->addWidget(iconBtn);
        connect(iconBtn, &QPushButton::clicked, this, [iconId, &chosenIconId]() {
            chosenIconId = iconId;
        });
    }
    layout->addWidget(new QLabel("Icon:", &dialog));
    layout->addLayout(iconLayout);

    QList<QPair<QUuid, QString>> allEntries = myRegistry->getAllUuids();
    QLineEdit *searchEdit = new QLineEdit(&dialog);
    searchEdit->setPlaceholderText("Search for a note...");
    QListWidget *resultsList = new QListWidget(&dialog);
    resultsList->setMaximumHeight(150);
    resultsList->hide();
    QUuid chosenUuid;
    if(pregenData != nullptr)
        chosenUuid = pregenData->Link;
    if(pregenData != nullptr && !pregenData->Link.isNull())
    {
        for(const auto &entry : allEntries)
        {
            if(entry.first == pregenData->Link)
            {
                searchEdit->setText(entry.second);
                break;
            }
        }
    }
    connect(searchEdit, &QLineEdit::textChanged, this,
            [&allEntries, resultsList, &chosenUuid](const QString &text) {
                resultsList->clear();

                chosenUuid = QUuid();
                if (text.trimmed().isEmpty()) {
                    resultsList->hide();
                    return;
                }
                int count = 0;
                for (const auto &entry : allEntries) {
                    if (entry.second.contains(text, Qt::CaseInsensitive)) {
                        QListWidgetItem *item = new QListWidgetItem(entry.second);
                        item->setData(Qt::UserRole, entry.first.toString(QUuid::WithoutBraces));
                        resultsList->addItem(item);
                        if (++count >= 10)
                            break;
                    }
                }
                resultsList->setVisible(count > 0);
            });
    connect(resultsList, &QListWidget::itemClicked, this,
            [searchEdit, resultsList, &chosenUuid](QListWidgetItem *item) {
                chosenUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                searchEdit->blockSignals(true);
                searchEdit->setText(item->text());
                searchEdit->blockSignals(false);
                resultsList->hide();
            });
    layout->addWidget(new QLabel("Link to note:", &dialog));
    layout->addWidget(searchEdit);
    layout->addWidget(resultsList);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
        MarkerData marker;
        marker.X = x;
        marker.Y = y;
        marker.Label = labelEdit->text();
        marker.Color = chosenColor.name();
        marker.IconId = chosenIconId;
        marker.Link = chosenUuid;
        marker.size = sizeEdit->text().toInt();
        return marker;
    }

    return MarkerData();
}

/*
ShapeData ImageAnnotationEditor::CreateShape(ShapeData *pregenData)
{

    QDialog dialog(this);
    dialog.setWindowTitle("Edit Shape");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    pregenData->Closed;
    pregenData->StyleOfFill.fillColor;
    pregenData->StyleOfFill.fillPatternId;
    pregenData->StyleOfLine.lineColor;
    pregenData->StyleOfLine.linePatternId;
    pregenData->StyleOfLine.width;
    pregenData->rounding;
    pregenData->XYPoints; // These are set by another part of the code, all else can be configured here.

    QLineEdit *labelEdit = new QLineEdit(&dialog);
    if(pregenData != nullptr)
        labelEdit->setText(pregenData);
    layout->addWidget(new QLabel("Label:", &dialog));
    layout->addWidget(labelEdit);

    QColor chosenColor("#FFFFFF");
    if(pregenData != nullptr)
        chosenColor = pregenData->Color;
    QPushButton *colorPick = new QPushButton(&dialog);
    colorPick->setStyleSheet("background-color: " + chosenColor.name() + ";");
    colorPick->setFixedHeight(24);
    connect(colorPick, &QPushButton::clicked, this, [&chosenColor, colorPick, &dialog]() {
        QColor c = QColorDialog::getColor(chosenColor, &dialog, "Pick a color");
        if (c.isValid()) {
            chosenColor = c;
            colorPick->setStyleSheet("background-color: " + c.name() + ";");
        }
    });
    layout->addWidget(new QLabel("Color:", &dialog));
    layout->addWidget(colorPick);

    QLineEdit *sizeEdit = new QLineEdit(&dialog);
    sizeEdit->setValidator(new QIntValidator(0, 999, this));
    if(pregenData != nullptr)
        sizeEdit->setText(QString::number( pregenData->size ));
    else
        sizeEdit->setText("16");
    layout->addWidget(new QLabel("Size:", &dialog));
    layout->addWidget(sizeEdit);

    QStringList iconIds = { "Circle", "D8" };
    QString chosenIconId = iconIds.first();
    if(pregenData != nullptr)
        chosenIconId = pregenData->IconId;
    QHBoxLayout *iconLayout = new QHBoxLayout();
    QButtonGroup *iconGroup = new QButtonGroup(&dialog);
    iconGroup->setExclusive(true);
    for (const QString &iconId : iconIds)
    {
        QPushButton *iconBtn = new QPushButton(&dialog);
        iconBtn->setCheckable(true);
        iconBtn->setFixedSize(36, 36);
        iconBtn->setIcon(QIcon(":/icons/markers/" + iconId + ".svg"));
        iconBtn->setIconSize(QSize(24, 24));
        if (iconId == chosenIconId)
            iconBtn->setChecked(true);
        iconGroup->addButton(iconBtn);
        iconLayout->addWidget(iconBtn);
        connect(iconBtn, &QPushButton::clicked, this, [iconId, &chosenIconId]() {
            chosenIconId = iconId;
        });
    }
    layout->addWidget(new QLabel("Icon:", &dialog));
    layout->addLayout(iconLayout);

    QList<QPair<QUuid, QString>> allEntries = myRegistry->getAllUuids();
    QLineEdit *searchEdit = new QLineEdit(&dialog);
    searchEdit->setPlaceholderText("Search for a note...");
    QListWidget *resultsList = new QListWidget(&dialog);
    resultsList->setMaximumHeight(150);
    resultsList->hide();
    QUuid chosenUuid;
    if(pregenData != nullptr)
        chosenUuid = pregenData->Link;
    if(pregenData != nullptr && !pregenData->Link.isNull())
    {
        for(const auto &entry : allEntries)
        {
            if(entry.first == pregenData->Link)
            {
                searchEdit->setText(entry.second);
                break;
            }
        }
    }
    connect(searchEdit, &QLineEdit::textChanged, this,
            [&allEntries, resultsList, &chosenUuid](const QString &text) {
                resultsList->clear();

                chosenUuid = QUuid();
                if (text.trimmed().isEmpty()) {
                    resultsList->hide();
                    return;
                }
                int count = 0;
                for (const auto &entry : allEntries) {
                    if (entry.second.contains(text, Qt::CaseInsensitive)) {
                        QListWidgetItem *item = new QListWidgetItem(entry.second);
                        item->setData(Qt::UserRole, entry.first.toString(QUuid::WithoutBraces));
                        resultsList->addItem(item);
                        if (++count >= 10)
                            break;
                    }
                }
                resultsList->setVisible(count > 0);
            });
    connect(resultsList, &QListWidget::itemClicked, this,
            [searchEdit, resultsList, &chosenUuid](QListWidgetItem *item) {
                chosenUuid = QUuid::fromString(item->data(Qt::UserRole).toString());
                searchEdit->blockSignals(true);
                searchEdit->setText(item->text());
                searchEdit->blockSignals(false);
                resultsList->hide();
            });
    layout->addWidget(new QLabel("Link to note:", &dialog));
    layout->addWidget(searchEdit);
    layout->addWidget(resultsList);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
        MarkerData marker;
        marker.X = x;
        marker.Y = y;
        marker.Label = labelEdit->text();
        marker.Color = chosenColor.name();
        marker.IconId = chosenIconId;
        marker.Link = chosenUuid;
        marker.size = sizeEdit->text().toInt();
        return marker;
    }

    return MarkerData();
}
*/

void ImageAnnotationEditor::ClickedMarker(MarkerItem *mark, bool shift)
{
    if(isEditingShapes)
        return;
    if(isEditingMarkers)
    {
        if (shift)
        {
            // Add marker to the selected list, remove marker from list if it is already selected
            if(mark->isSelected)
            {
                for(int i = 0; i < selectedMarkers->count(); i++)
                {
                    if((*selectedMarkers)[i] == mark)
                    {
                        (*selectedMarkers)[i]->SelectMe(false);
                        selectedMarkers->removeAt(i);
                    }
                }
            }
            else
            {
                selectedMarkers->append(mark);
                mark->SelectMe(true);
            }
        }
        else
        {
            // Select only this marker, clear selected list
            for(int i = 0; i < selectedMarkers->count(); i++)
            {
                (*selectedMarkers)[i]->SelectMe(false);
            }
            selectedMarkers->clear();
            selectedMarkers->append(mark);
            mark->SelectMe(true);
        }
    }
    else
    {
        emitUuid(mark->myData->Link.toString());
    }
}

void ImageAnnotationEditor::RightClickedMarker(MarkerItem *mark)
{
    if(isEditingShapes)
        return;
    if(isEditingMarkers)
    {
        MarkerData New = CreateMarker(mark->myData->X, mark->myData->Y, mark->myData);
        if(New.IconId.isEmpty())
            return;
        for(int i = 0; i < myData->markers.count(); i++)
        {
            if(myData->markers[i] == *mark->myData)
            {
                myData->markers[i] = New;
            }
        }
        UpdateMarkers();
    }
}
void ImageAnnotationEditor::RightClickedShape(ShapeGraphicsObject *shape)
{
    if(isEditingMarkers)
        return;
    if(isEditingShapes)
    {
        //ShapeData New = CreateShape(shape->MyData);
        //Detect if the New is empty:
        //if(New.IconId.isEmpty())
        //    return;
        for(int i = 0; i < myData->shapes.count(); i++)
        {
            if(myData->shapes[i] == *shape->MyData)
            {
                //myData->shapes[i] = New;
            }
        }
        UpdateShapes();
    }
}

void ImageAnnotationEditor::emitUuid(QString uuid)
{
    emit uuidClicked(QUuid::fromString(uuid));
}

void ImageAnnotationEditor::dragMarkers(QPointF delta)
{
    for(int i = 0; i < selectedMarkers->count(); i++)
    {
        (*selectedMarkers)[i]->setPos(QPointF((*selectedMarkers)[i]->myData->X * imageItem->pixmap().width(), (*selectedMarkers)[i]->myData->Y * imageItem->pixmap().height()) + delta);
    }
    graphicsView->viewport()->update();
}

void ImageAnnotationEditor::finishDragging(QPointF delta)
{
    graphicsView->panningLocked = false;
    for(int i = 0; i < selectedMarkers->count(); i++)
    {
        (*selectedMarkers)[i]->setPos(QPointF((*selectedMarkers)[i]->myData->X * imageItem->pixmap().width(), (*selectedMarkers)[i]->myData->Y * imageItem->pixmap().height()) + delta);
        (*selectedMarkers)[i]->myData->X = (*selectedMarkers)[i]->pos().x() / imageItem->pixmap().width();
        (*selectedMarkers)[i]->myData->Y = (*selectedMarkers)[i]->pos().y() / imageItem->pixmap().height();
    }
    graphicsView->viewport()->update();
}

void ImageAnnotationEditor::on_actionShapesEditMode_toggled(bool arg1)
{
    if(arg1)
    {
        ui->EditShapesToolBar->show();
        isEditingShapes = true;
        ui->actionMoveMarkers->setChecked(false);
    }
    else
    {
        ui->EditShapesToolBar->hide();
        isEditingShapes = false;
    }
}

void ImageAnnotationEditor::on_actionMoveMarkers_toggled(bool arg1)
{
    if(arg1)
    {
        ui->EditMarkersToolBar->show();
        isEditingMarkers = true;
        ui->actionShapesEditMode->setChecked(false);
    }
    else
    {
        graphicsView->panningLocked = false;
        for(int i = selectedMarkers->count() - 1; i >= 0; i--)
        {
            (*selectedMarkers)[i]->SelectMe(false);
            (*selectedMarkers).removeAt(i);
        }
        ui->EditMarkersToolBar->hide();
        isEditingMarkers = false;
    }
}

void ImageAnnotationEditor::on_actionAddMarker_triggered()
{
    if(isEditingMarkers)
    {
        QPoint mousePos = graphicsView->viewport()->mapFromGlobal(QCursor::pos());
        QPointF scenePos;
        if (graphicsView->viewport()->rect().contains(mousePos))
        {
            scenePos = graphicsView->mapToScene(mousePos);
        }
        else
        {
            scenePos = graphicsView->mapToScene(graphicsView->viewport()->rect().center());
        }
        MarkerData marker = CreateMarker(scenePos.x() / imageItem->pixmap().width(), scenePos.y() / imageItem->pixmap().height());
        if(marker.IconId.isEmpty())
            return;
        myData->markers.append(marker);
        UpdateMarkers();
    }
}

void ImageAnnotationEditor::on_actionRemoveMarker_triggered()
{
    if(isEditingMarkers)
    {
        for(int i = selectedMarkers->count()-1; i >= 0; i--)
        {
            for(int o = myData->markers.count()-1; o >= 0; o--)
            {
                if(myData->markers[o] == *(*selectedMarkers)[i]->myData)
                {
                    graphicsView->scene()->removeItem((*selectedMarkers)[i]);
                    (*selectedMarkers)[i]->deleteLater();
                    myData->markers.removeAt(o);
                    selectedMarkers->removeAt(i);
                    break;
                }
            }
        }
    }
}
