# -------------------------------------------------
# Project created by QtCreator 2010-06-22T20:05:13
# -------------------------------------------------
TARGET = pixeleasel
TEMPLATE = app
SOURCES += main.cpp \
    ImageView.cpp \
    HotkeyBar.cpp \
    ImagePreview.cpp \
    Palette.cpp \
    PaletteColour.cpp \
    PaletteWidget.cpp \
    PaletteColourWidget.cpp \
    Layers/Layer.cpp \
    Layers/BackgroundLayer.cpp \
    Layers/SolidBackgroundLayer.cpp \
    Tools/Tool.cpp \
    Tools/LineTool.cpp \
    Tools/PenTool.cpp \
    Tools/SelectTool.cpp \
    FlowLayout.cpp
SOURCES += ResizeDialog.cpp
SOURCES += Commands.cpp
SOURCES += PixelEasel.cpp
SOURCES += ImageCanvas.cpp
SOURCES += ImageDocument.cpp
HEADERS += PixelEasel.h \
    ImageView.h \
    HotkeyBar.h \
    ImagePreview.h \
    Palette.h \
    PaletteColour.h \
    PaletteWidget.h \
    PaletteColourWidget.h \
    FlowLayout.h
HEADERS += Layers/Layer.h \
    Layers/BackgroundLayer.h \
    Layers/SolidBackgroundLayer.h
HEADERS += Tools/Tool.h \
    Tools/LineTool.h \
    Tools/PenTool.h \
    Tools/SelectTool.h
HEADERS += ResizeDialog.h
HEADERS += Commands.h
HEADERS += ImageCanvas.h
HEADERS += ImageDocument.h

OTHER_FILES += \
    images/pencil.png \
    images/line.png
