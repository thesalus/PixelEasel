#include "ImageDocument.h"
#include "Commands.h"
#include <QFileDialog>
#include <QDir>
#include <QHash>

int ImageDocument::untitled_counter = 0;

ImageDocument::ImageDocument(QSize size)
    :	file_name("Untitled"+(untitled_counter > 0 ? " "+QString::number(untitled_counter+1) : "")),
        has_file_flag(false),
        selection_changed_flag(false),
        emptyScratchpadSelection(true),
        image_index(0),
	scratchpad(size, QImage::Format_ARGB32),
        undo_stack(NULL),
        palette(new Palette)
{
    // create a transparent background
    QImage image(size, QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0));
        image_layers << (new Layer(image));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    untitled_counter++;
    initialize();
}

ImageDocument::ImageDocument(QString new_file_name)
    :	file_name(new_file_name),
        has_file_flag(true),
        selection_changed_flag(false),
        emptyScratchpadSelection(true),
        image_index(0),
	scratchpad(QSize(1,1), QImage::Format_ARGB32),
        undo_stack(NULL),
        palette(new Palette)
{
    QImage image(file_name);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    if (image.isNull())
    {
	QMessageBox::information(this, tr("Image Canvas"),
                                 tr("Cannot load %1.").arg(file_name));
	this->close();
    }
    else
    {
	scratchpad = QImage(image.size(), QImage::Format_ARGB32);
            scratchpad.fill(qRgba(0, 0, 0, 0));
        image_layers << (new Layer(image));
	initialize();
    }
}

// create from clipboard?

void ImageDocument::initialize()
{
    ImageCanvas *canvas = new ImageCanvas(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
            canvas,	    SLOT(refreshImage(const QImage&)));
    connect(canvas,	    SIGNAL(selectionModified()),
            this,	    SLOT(passSelectionModified()));
    views << canvas;

    scratchpad_translation = QPoint(0,0);

    scroll_area = new QScrollArea;
    scroll_area->setBackgroundRole(QPalette::Base);
    scroll_area->setAlignment(Qt::AlignCenter);
    scroll_area->setStyleSheet("QLabel { background-color: #999999 }");
    scroll_area->setWidget(canvas);

    myPenWidth = 1;
    myPenColor = Qt::black;
    myPen = QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // grab all the colours from each layer and put it in the palette
    // TODO: make the list unique
    QHash<QRgb, int>     colour_hash;
    for (int i = 0; i < image_layers.size(); ++i) {
        Layer *image = image_layers.at(i);
        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                QRgb key = image->pixel(x,y);
                if (key != Qt::transparent && colour_hash.value(key, 0) == 0) {
                    colourTable.push_back(key);
                    colour_hash[key] = 1;
                    palette->addColour(key);
                }
            }
        }
    }
    for (int i = 0; i < image_layers.size(); ++i) {
        image_layers.at(i)->setColorTable(colourTable);
    }

    preview = new ImagePreview(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
            preview,	    SLOT(refreshImage(const QImage&)));

    this->setWidget(scroll_area);
    this->setWindowTitle(file_name.split("/").last());
    this->resize(128, 128);
    canvas->show();
    this->makeChange();
}

ImageDocument::~ImageDocument()
{
    delete undo_stack;
    delete scroll_area;
}

void ImageDocument::scaleImage(double scale_factor)
{
    // TODO: how do we do this part? separate subwindow?
    ((ImageCanvas*) views.first())->scaleImage(scale_factor);
}

void ImageDocument::resetScale()
{
    ((ImageCanvas*) views.first())->resetScale();
}

void ImageDocument::save()
{
    // Expand this to include other file types
    // TODO: do we need to make use of has_file_flag
    if (this->getImage()->save(file_name, "PNG"))
    {
        if (undo_stack != NULL && !undo_stack->isClean())
            undo_stack->setClean();
    }
    makeChange();
}

QImage* ImageDocument::getImage()
{
//    scratchpad = Layer(image.size(), QImage::Format_ARGB32);
    Layer image(scratchpad.size(),  QImage::Format_ARGB32);
    for (int i = 0; i < image_layers.size(); ++i) {
        image = image_layers.at(i)->layOver(image);
    }
    Layer temp(scratchpad.size(), QImage::Format_ARGB32);
    QPainter painter(&temp);
        painter.drawImage(scratchpad_translation, scratchpad);
        painter.end();

    QImage *new_image = new QImage(temp.layOver(image));
    return new_image;
}

Palette* ImageDocument::getPalette()
{
    return palette;
}

QString ImageDocument::getPath()
{
    return file_name;
}

ImagePreview* ImageDocument::getPreview()
{
    return preview;
}

QSize ImageDocument::getSize()
{
    // is this the best size to return?
    return scratchpad.size();
}

QUndoStack* ImageDocument::getUndoStack()
{
    return undo_stack;
}

void ImageDocument::replaceImage(QImage new_image)
{
    image_layers.replace(image_index, new Layer(new_image));
    this->makeChange();
}

void ImageDocument::drawImage(QImage new_image)
{
    QPainter painter(image_layers.at(image_index));
        painter.drawImage(QPoint(0,0), new_image);
        painter.end();
    this->makeChange();
}

void ImageDocument::drawLines(QVector<QPoint> point_pairs)
{
    QImage old_image = image_layers.at(image_index)->copy();
    QPainter painter(image_layers.at(image_index));
        painter.setPen(myPen);
        painter.drawLines(point_pairs);
	painter.end();
    if (undo_stack != NULL)
    {
        AddCommand * command = new AddCommand(old_image, *image_layers.at(image_index), this);
	command->setText("Draw Line");
        undo_stack->push(command);
    }
    refreshScratchpad();
    this->makeChange();
}

void ImageDocument::refreshScratchpad() {
    scratchpad = Layer(image_layers.at(image_index)->size(), QImage::Format_ARGB32);
    this->makeChange();
}

void ImageDocument::scratchLine(QPoint start_point, QPoint end_point)
{
    QPainter painter(&scratchpad);
        painter.setPen(myPen);
        painter.drawLine(start_point, end_point);
	painter.end();
    //    int rad = (pen.width() / 2) + 2;
    //    this->update(QRect(startPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));

    this->makeChange();
}

void ImageDocument::makeChange()
{
    emit imageModified(*getImage());
}

void ImageDocument::updateTitle(bool state)
{
    QString title = file_name.split("/").last();
    if (!state)
	title += "*";
    this->setWindowTitle(title);
}

void ImageDocument::setFileName(QString file)
{
    file_name = file;
    has_file_flag = true;
    makeChange();
    // TODO: we're duplicating the makechange call here when saving as...
}

void ImageDocument::setSize(QSize new_size)
{
    Layer newImage(new_size, QImage::Format_ARGB32);
    QPainter painter(&newImage);
        painter.drawImage(QPoint(0,0), *image_layers.at(image_index));
        painter.end();
    if (undo_stack != NULL)
    {
        AddCommand* command = new AddCommand(*image_layers.at(image_index), newImage, this);
	command->setText("Resize Image");
        undo_stack->push(command);
    }
    image_layers.replace(image_index, new Layer(newImage));
    this->makeChange();
}

void ImageDocument::setUndoStack(QUndoStack* new_undo_stack)
{
    undo_stack = new_undo_stack;
    connect(undo_stack,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateTitle(bool)));
}

void ImageDocument::closeEvent(QCloseEvent* event)
{
    // figure out a way to make this more convenient for users when dealing with multiple files.
    if (!undo_stack->isClean())
    {
        switch( QMessageBox::information( this, "Unsaved Changes in "+file_name.split("/").last(),
                                              "The document has been changed since "
                                              "the last save.",
                                              "&Save Now", "&Discard Changes", "&Cancel",
                                              0, 1 ) )
        {
            case 0:
                // if there is no filename, then query the user for one
                if (!has_file_flag)
                {
                    QString new_file_name = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    QDir::currentPath(),
                                                    tr("Images (*.png *.gif)"));
                    if (new_file_name.isEmpty())
                    {
                        event->ignore();    // TODO: should we ask them again?
                        break;
                    }
                    setFileName(new_file_name);
                }
                save();
                event->accept();
                break;
            case 1:
                event->accept();
                break;
            case 2:
            default:
                event->ignore();
                break;
        }
    }
    else
    {
        event->accept();
    }
}

bool ImageDocument::hasFile()
{
    return has_file_flag;
}

void ImageDocument::setToolInActiveView(Tool::ToolTypes type)
{
    if (type != Tool::SelectTool)
    {
        // what if we change the layers, do we discard the selection?
        // TODO: for each of them
        views.first()->showSelection(false);
    }
    views.first()->setTool(type);
}

void ImageDocument::setColour(PaletteColour* colour)
{
    // TODO: add if it does not exist in the colour table?
    //          then tell the PaletteWidget so that it may add it.
    QRgb rgb = colour->getRGBA();
    myPenColor = rgb;
    myPen.setColor(myPenColor);
    if (colourTable.indexOf(rgb) == -1) {
        colourTable.push_back(rgb);
        for (int i = 0; i < image_layers.size(); ++i) {
            image_layers.at(i)->setColorTable(colourTable);
        }
        palette->addColour(rgb);
    }
}

bool ImageDocument::hasSelection()
{
    // look in the _ACTIVE_ view
    return views.first()->hasSelection();
}

void ImageDocument::clearRect(QRect rect)
{
    QPainter painter(image_layers.at(image_index));
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.eraseRect(rect);
        painter.end();
    this->makeChange();
}

void ImageDocument::cut(QClipboard *clipboard)
{
    // TODO BUG: we need to merge the scratchpad first
    if (views.first()->hasSelection())
    {
        QImage oldImage = image_layers.at(image_index)->copy(image_layers.at(image_index)->rect());
        QRect selection = views.first()->getSelection();
        clipboard->setImage(image_layers.at(image_index)->copy(selection));
        views.first()->showSelection(false);
        clearRect(selection);
        if (undo_stack != NULL)
        {
            AddCommand * command = new AddCommand(oldImage, *image_layers.at(image_index), this);
            command->setText("Cut");
            undo_stack->push(command);
        }
    }
}

void ImageDocument::copy(QClipboard *clipboard)
{
    if (views.first()->hasSelection())
    {
        QRect selection = views.first()->getSelection();
        clipboard->setImage(image_layers.at(image_index)->copy(selection));
    }
}

void ImageDocument::paste(QClipboard *clipboard)
{
    if(clipboard->mimeData()->hasImage())
    {
        preScratch = image_layers.at(image_index)->copy(image_layers.at(image_index)->rect());
        QPainter painter(&scratchpad);
            painter.drawImage(QPoint(0,0), clipboard->image());
            painter.end();
        setSelection(QRect(QPoint(0,0), clipboard->image().size()));
        emptyScratchpadSelection = false;
        // the pasting action gets recorded when they actually release the selection
    }
}

void ImageDocument::clearSelection()
{
    if (views.first()->hasSelection())
    {
        QImage oldImage = image_layers.at(image_index)->copy(image_layers.at(image_index)->rect());
        QRect selection = views.first()->getSelection();
        views.first()->showSelection(false);
        clearRect(selection);
        if (undo_stack != NULL)
        {
            AddCommand * command = new AddCommand(oldImage, *image_layers.at(image_index), this);
            command->setText("Delete");
            undo_stack->push(command);
        }
    }
}

void ImageDocument::setSelection(QRect rect)
{
    views.first()->setSelectBox(rect);
    selection_changed_flag = true;
    emit makeChange();
}

void ImageDocument::passSelectionModified()
{
    // merge the selected image back onto the original if it was changed
    if (selection_changed_flag)
    {
        QPainter painter(image_layers.at(image_index));
            painter.drawImage(scratchpad_translation, scratchpad);
            painter.end();
        refreshScratchpad();
        if (undo_stack != NULL)
        {
            AddCommand * command = new AddCommand(preScratch, *image_layers.at(image_index), this);
            command->setText("Merge Selection");
            undo_stack->push(command);
        }
        // TODO: don't put this on the stack if we cancel
        selection_changed_flag = false;
        emptyScratchpadSelection = true;
        scratchpad_translation = QPoint(0,0);
    }
    emit selectionModified(hasSelection());
}

// TODO: close if the primary view is closed?

void ImageDocument::selectToScratchpad()
{
    // is it a bad idea to clip it out of the image? Yes.
    QRect rect = views.first()->getSelection();
    preScratch = image_layers.at(image_index)->copy();
    QImage moved = image_layers.at(image_index)->copy(rect);
    clearRect(rect);
    QPainter painter(&scratchpad);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.eraseRect(scratchpad.rect());
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(rect.topLeft(), moved);
        painter.end();
    emptyScratchpadSelection = false;
    selection_changed_flag = true;
    emit makeChange();
}

void ImageDocument::translateSelection(QPoint point)
{
    if (emptyScratchpadSelection)
    {
        selectToScratchpad();
    }
    scratchpad_translation += point;
    emit makeChange();
}
