# -------------------------------------------------
# Project created by QtCreator 2010-06-22T20:05:13
# -------------------------------------------------
TARGET = pixeleasel
TEMPLATE = app
SOURCES += main.cpp \
    ImageView.cpp \
    ImagePreview.cpp \
    Palette.cpp \
    PaletteColour.cpp \
    FlowLayout.cpp \
    ActionsDialog.cpp \
    ImageDocument.cpp
SOURCES += Layers/Layer.cpp \
    Layers/BackgroundLayer.cpp \
    Layers/SolidBackgroundLayer.cpp
SOURCES += Tools/Tool.cpp \
    Tools/LineTool.cpp \
    Tools/PenTool.cpp \
    Tools/SelectTool.cpp \
    Tools/EyedropTool.cpp
SOURCES += Widgets/HotkeyBar.cpp \
    Widgets/PaletteWidget.cpp \
    Widgets/PaletteColourWidget.cpp \
    Widgets/ZoomWidget.cpp
SOURCES += ResizeDialog.cpp
SOURCES += Commands.cpp
SOURCES += PixelEasel.cpp
SOURCES += ImageCanvas.cpp
HEADERS += PixelEasel.h \
    ImageView.h \
    ImagePreview.h \
    Palette.h \
    PaletteColour.h \
    FlowLayout.h \
    ActionsDialog.h
HEADERS += Widgets/HotkeyBar.h \
    Widgets/PaletteWidget.h \
    Widgets/PaletteColourWidget.h \
    Widgets/ZoomWidget.h
HEADERS += Layers/Layer.h \
    Layers/BackgroundLayer.h \
    Layers/SolidBackgroundLayer.h
HEADERS += Tools/Tool.h \
    Tools/EyedropTool.h \
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
