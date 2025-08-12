
#include <QApplication>
#include <QStyle>
#include <QtPlugin>

#include <iostream>

#include "mainwindow.h"
#include "plugin.h"
#include "ThemeInfo.h"

// Import Qt static plugins (required for static platform plugins)
#ifdef Q_OS_WIN
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#endif

// Import Kvantum headers for direct style creation
#include "Kvantum.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // Always start with the Default theme
    const ThemeInfo* themeInfo = getThemeInfo(ThemeType::Default);
    std::cout << "Loading " << themeInfo->name << " theme..." << std::endl;
    
    // Initialize default theme resources
    Q_INIT_RESOURCE(Default);
    
    // Create Kvantum style with default theme configuration
    Kvantum::Style* kvantumStyle = new Kvantum::Style(
        themeInfo->configPath,
        themeInfo->svgPath,
        themeInfo->colorPath
    );
    
    if (kvantumStyle && kvantumStyle->isLoaded())
    {
        QApplication::setStyle(kvantumStyle);
        std::cout << "Successfully applied Kvantum style!" << std::endl;
        QApplication::setPalette(kvantumStyle->standardPalette());
    }
    else
    {
        std::cout << "Failed to create kvantum style.\n";
        std::cout << "Unable to load theme configuration.\n";
        if (kvantumStyle) delete kvantumStyle;
        return 1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
