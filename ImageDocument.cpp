#include "ImageDocument.h"
#include "Commands.h"
#include <QFileDialog>
#include <QDir>

int ImageDocument::c_untitled_documents = 0;

ImageDocument::ImageDocument(QSize size)
    :	fileName("Untitled"+(c_untitled_documents > 0 ? " "+QString::number(c_untitled_documents+1) : "")),
        isSavedToFile(false),
        isSelectionChanged(false),
        isScratchpadSelectionEmpty(true),
        imageIndex(0),
	scratchpad(size, QImage::Format_ARGB32),
        undoStack(NULL),
        preview(NULL),
        palette(new Palette)
{
    // create a transparent background
    QImage image(size, QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0));
        imageLayers << (new Layer(image));
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    c_untitled_documents++;
    initialize();
}

ImageDocument::ImageDocument(QString new_fileName)
    :	fileName(new_fileName),
        isSavedToFile(true),
        isSelectionChanged(false),
        isScratchpadSelectionEmpty(true),
        imageIndex(0),
	scratchpad(QSize(1,1), QImage::Format_ARGB32),
        undoStack(NULL),
        preview(NULL),
        palette(new Palette)
{
    QImage image(fileName);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    if (image.isNull())
    {
	QMessageBox::information(this, tr("Image Canvas"),
                                 tr("Cannot load %1.").arg(fileName));
	this->close();
    }
    else
    {
	scratchpad = QImage(image.size(), QImage::Format_ARGB32);
            scratchpad.fill(qRgba(0, 0, 0, 0));
        imageLayers << (new Layer(image));
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

    scratchpadTranslation = QPoint(0,0);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("QLabel { background-color: #999999 }");
    scrollArea->setWidget(canvas);

    m_pen_width = 1;
    m_pen_colour = Qt::black;
    m_pen = QPen(m_pen_colour, m_pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    initializeColourTable();
    preview = new ImagePreview(this);
    connect(this,	    SIGNAL(imageModified(const QImage&)),
            preview,	    SLOT(refreshImage(const QImage&)));

    this->setWidget(scrollArea);
    this->setWindowTitle(fileName.split("/").last());
    this->resize(128, 128);
    canvas->show();
    this->makeChange();
}

void ImageDocument::initializeColourTable()
{
    // grab all the colours from each layer and put it in the palette
    QHash<QRgb, int>     colour_hash;
    for (int i = 0; i < imageLayers.size(); ++i) {
        Layer *image = imageLayers.at(i);
        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                QRgb key = image->pixel(x,y);
                addColourToTable(key, colour_hash);
            }
        }
    }
    for (int i = 0; i < imageLayers.size(); ++i) {
        imageLayers.at(i)->setColorTable(colourTable);
    }
}

void ImageDocument::addColourToTable(QRgb key, QHash<QRgb, int> & colourHash)
{
    bool isNewColour = key != Qt::transparent && colourHash.value(key, 0) == 0;
    if (isNewColour) {
        colourTable.push_back(key);
        colourHash[key] = 1;
        palette->addColour(key);
    }
}

ImageDocument::~ImageDocument()
{
    delete undoStack;
    delete palette;
    delete preview;
    delete scrollArea;
}

void ImageDocument::resetScale()
{
    ((ImageCanvas*) views.first())->resetScale();
}

void ImageDocument::save()
{
    // Expand this to include other file types
    // TODO: do we need to make use of isSavedToFile
    if (this->getImage()->save(fileName, "PNG")) {
        if (undoStack != NULL && !undoStack->isClean()) {
            undoStack->setClean();
        }
    }
    makeChange();
}

QImage* ImageDocument::getImage()
{
//    scratchpad = Layer(image.size(), QImage::Format_ARGB32);
    Layer image(scratchpad.size(),  QImage::Format_ARGB32);
    for (int i = 0; i < imageLayers.size(); ++i) {
        image = imageLayers.at(i)->layOver(image);
    }
    Layer temp(scratchpad.size(), QImage::Format_ARGB32);
    QPainter painter(&temp);
        painter.drawImage(scratchpadTranslation, scratchpad);
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
    return fileName;
}

QRgb ImageDocument::getPixel(QPoint point)
{
    return imageLayers.at(imageIndex)->pixel(point);
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
    return undoStack;
}

void ImageDocument::replaceImage(QImage new_image)
{
    imageLayers.replace(imageIndex, new Layer(new_image));
    this->makeChange();
}

void ImageDocument::drawImage(QImage new_image)
{
    QPainter painter(imageLayers.at(imageIndex));
        painter.drawImage(QPoint(0,0), new_image);
        painter.end();
    this->makeChange();
}

void ImageDocument::drawLines(QVector<QPoint> point_pairs)
{
    QImage old_image = imageLayers.at(imageIndex)->copy();
    QPainter painter(imageLayers.at(imageIndex));
        painter.setPen(m_pen);
        painter.drawLines(point_pairs);
	painter.end();
    if (undoStack != NULL)
    {
        AddCommand * command = new AddCommand(old_image, *imageLayers.at(imageIndex), this);
	command->setText("Draw Line");
        undoStack->push(command);
    }
    refreshScratchpad();
    this->makeChange();
}

void ImageDocument::refreshScratchpad() {
    scratchpad = Layer(imageLayers.at(imageIndex)->size(), QImage::Format_ARGB32);
    this->makeChange();
}

void ImageDocument::scratchLine(QPoint start_point, QPoint end_point)
{
    QPainter painter(&scratchpad);
        painter.setPen(m_pen);
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
    QString title = fileName.split("/").last();
    if (!state)
	title += "*";
    this->setWindowTitle(title);
}

void ImageDocument::setFileName(QString file)
{
    fileName = file;
    isSavedToFile = true;
    makeChange();
    // TODO: we're duplicating the makechange call here when saving as...
}

void ImageDocument::setSize(QSize new_size)
{
    Layer newImage(new_size, QImage::Format_ARGB32);
    QPainter painter(&newImage);
        painter.drawImage(QPoint(0,0), *imageLayers.at(imageIndex));
        painter.end();
    if (undoStack != NULL) {
        AddCommand* command = new AddCommand(*imageLayers.at(imageIndex), newImage, this);
	command->setText("Resize Image");
        undoStack->push(command);
    }
    imageLayers.replace(imageIndex, new Layer(newImage));
    this->makeChange();
}

void ImageDocument::setUndoStack(QUndoStack* new_undoStack)
{
    undoStack = new_undoStack;
    connect(undoStack,	    SIGNAL(cleanChanged(bool)),
	    this,	    SLOT(updateTitle(bool)));
}

void ImageDocument::closeEvent(QCloseEvent* event)
{
    // figure out a way to make this more convenient for users when dealing with multiple files.
    if (!undoStack->isClean()) {
        int option = QMessageBox::information( this, "Unsaved Changes in "+fileName.split("/").last(),
                                                "The document has been changed since "
                                                "the last save.",
                                                "&Save Now", "&Discard Changes", "&Cancel",
                                                0, 1 );
        switch( option ) {
            case 0:
                // if there is no filename, then query the user for one
                if (!isSavedToFile) {
                    QString new_fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    QDir::currentPath(),
                                                    tr("Images (*.png *.gif)"));
                    if (new_fileName.isEmpty()) {
                        event->ignore();    // TODO: should we ask them again?
                        break;
                    }
                    setFileName(new_fileName);
                }
                save();
                event->accept();
                break;
            case 1:
                event->accept();
                break;
            case 2:     // drop through
            default:
                event->ignore();
                break;
        }
    }
    else {
        event->accept();
    }
}

bool ImageDocument::hasFile()
{
    return isSavedToFile;
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

int ImageDocument::getZoomInActiveView()
{
    return ((ImageCanvas*) views.first())->getScale();
}

void ImageDocument::setZoomInActiveView(int scale)
{
    ((ImageCanvas*) views.first())->setScale(scale);
}

void ImageDocument::setColour(QRgb colour)
{
    m_pen_colour = colour;
    m_pen.setColor(m_pen_colour);
    if (colourTable.indexOf(colour) == -1) {
        colourTable.push_back(colour);
        for (int i = 0; i < imageLayers.size(); ++i) {
            imageLayers.at(i)->setColorTable(colourTable);
        }
        palette->addColour(colour);
    }
}

void ImageDocument::setColour(PaletteColour* colour)
{
    // TODO: add if it does not exist in the colour table?
    //          then tell the PaletteWidget so that it may add it.
    setColour(colour->getRGBA());
}

bool ImageDocument::hasSelection()
{
    // look in the _ACTIVE_ view
    return views.first()->hasSelection();
}

void ImageDocument::clearRect(QRect rect)
{
    QPainter painter(imageLayers.at(imageIndex));
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
        QImage oldImage = imageLayers.at(imageIndex)->copy(imageLayers.at(imageIndex)->rect());
        QRect selection = views.first()->getSelection();
        clipboard->setImage(imageLayers.at(imageIndex)->copy(selection));
        views.first()->showSelection(false);
        clearRect(selection);
        if (undoStack != NULL) {
            AddCommand * command = new AddCommand(oldImage, *imageLayers.at(imageIndex), this);
            command->setText("Cut");
            undoStack->push(command);
        }
    }
}

void ImageDocument::copy(QClipboard *clipboard)
{
    if (views.first()->hasSelection())
    {
        QRect selection = views.first()->getSelection();
        clipboard->setImage(imageLayers.at(imageIndex)->copy(selection));
    }
}

void ImageDocument::paste(QClipboard *clipboard)
{
    if(clipboard->mimeData()->hasImage())
    {
        preScratch = imageLayers.at(imageIndex)->copy(imageLayers.at(imageIndex)->rect());
        QPainter painter(&scratchpad);
            painter.drawImage(QPoint(0,0), clipboard->image());
            painter.end();
        setSelection(QRect(QPoint(0,0), clipboard->image().size()));
        isScratchpadSelectionEmpty = false;
        // the pasting action gets recorded when they actually release the selection
    }
}

void ImageDocument::clearSelection()
{
    if (views.first()->hasSelection())
    {
        QImage oldImage = imageLayers.at(imageIndex)->copy(imageLayers.at(imageIndex)->rect());
        QRect selection = views.first()->getSelection();
        views.first()->showSelection(false);
        clearRect(selection);
        if (undoStack != NULL) {
            AddCommand * command = new AddCommand(oldImage, *imageLayers.at(imageIndex), this);
            command->setText("Delete");
            undoStack->push(command);
        }
    }
}

void ImageDocument::setSelection(QRect rect)
{
    views.first()->setSelectBox(rect);
    isSelectionChanged = true;
    emit makeChange();
}

void ImageDocument::passSelectionModified()
{
    // merge the selected image back onto the original if it was changed
    if (isSelectionChanged)
    {
        QPainter painter(imageLayers.at(imageIndex));
            painter.drawImage(scratchpadTranslation, scratchpad);
            painter.end();
        refreshScratchpad();
        if (undoStack != NULL) {
            AddCommand * command = new AddCommand(preScratch, *imageLayers.at(imageIndex), this);
            command->setText("Merge Selection");
            undoStack->push(command);
        }
        // TODO: don't put this on the stack if we cancel
        isSelectionChanged = false;
        isScratchpadSelectionEmpty = true;
        scratchpadTranslation = QPoint(0,0);
    }
    emit selectionModified(hasSelection());
}

// TODO: close if the primary view is closed?

void ImageDocument::selectToScratchpad()
{
    // is it a bad idea to clip it out of the image? Yes.
    QRect rect = views.first()->getSelection();
    preScratch = imageLayers.at(imageIndex)->copy();
    QImage moved = imageLayers.at(imageIndex)->copy(rect);
    clearRect(rect);
    QPainter painter(&scratchpad);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.eraseRect(scratchpad.rect());
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(rect.topLeft(), moved);
        painter.end();
    isScratchpadSelectionEmpty = false;
    isSelectionChanged = true;
    emit makeChange();
}

void ImageDocument::translateSelection(QPoint point)
{
    if (isScratchpadSelectionEmpty) {
        selectToScratchpad();
    }
    scratchpadTranslation += point;
    emit makeChange();
}
