#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QAbstractTableModel>
#include <QABstractListModel>
#include <qstylefactory>
#include <QTimer>

#include <Kvantum.h>
#include "ThemeInfo.h"

#include <vector>

#define USEKVANTUM

struct Fruit {
    QString name;
    QString color;
};

class TableModel : public QAbstractTableModel
{
public:
    TableModel()
    {
        mFruits.push_back({ "Banana", "Yellow" });
        mFruits.push_back({ "Apple", "Green" });
        mFruits.push_back({ "Orange", "Orange" });
    }
    virtual QVariant data(const QModelIndex& index, int role) const override
    {
        if (role == Qt::DisplayRole)
        {
            if (index.column() == 0)
                return mFruits[index.row()].name;
            else if (index.column() == 1)
                return mFruits[index.row()].color;
        }
        return QVariant();
    }
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            if (section == 0) return "Name";
            else if (section == 1) return "Color";
        }
        return QVariant();
    }
    virtual int rowCount(const QModelIndex&) const override
    {
        return mFruits.size();
    }
    virtual int columnCount(const QModelIndex&) const override
    {
        return 2;
    }
private:
    std::vector<Fruit> mFruits;

};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Table/list models removed since Views tab was removed

    // Populate dropdown with all available embedded themes
    for (const auto& themeInfo : themeInfos)
    {
        ui->cmbSvgThemes->addItem(themeInfo.name);
    }
    
    // Set the current theme in the dropdown based on the active style
    Kvantum::Style* currentStyle = qobject_cast<Kvantum::Style*>(qApp->style());
    if (currentStyle)
    {
        // Default to first item if we can't determine current theme
        ui->cmbSvgThemes->setCurrentIndex(0);
        ui->lblTheme->setText(QString("Current Theme: %1").arg(ui->cmbSvgThemes->currentText()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ClearAllStylesheets()
{
    // Clear application stylesheet
    qApp->setStyleSheet("");
    
    // Clear all widget-specific stylesheets and reset palettes
    foreach (QWidget* widget, qApp->allWidgets())
    {
        if (widget)
        {
            widget->setStyleSheet("");
            // Reset widget palette to system default
            widget->setPalette(qApp->style()->standardPalette());
        }
    }
}

void MainWindow::ForceWidgetUpdate()
{
    // Force all widgets to update with new theme
    foreach (QWidget* widget, qApp->allWidgets())
    {
        if (widget)
        {
            widget->style()->unpolish(widget);
            widget->style()->polish(widget);
            widget->update();
        }
    }
}

void MainWindow::RestyleWindow()
{
    const QWidgetList topLevels = QApplication::topLevelWidgets();
    for (QWidget* widget : topLevels)
    { // this is needed with Qt >= 5.13.1 but is harmless otherwise
        widget->setAttribute(Qt::WA_NoSystemBackground, false);
        widget->setAttribute(Qt::WA_TranslucentBackground, false);
    }

    // Qt5 has QEvent::ThemeChange
    const QWidgetList widgets = QApplication::allWidgets();
    for (QWidget* widget : widgets)
    {
        QEvent event(QEvent::ThemeChange);
        QApplication::sendEvent(widget, &event);
    }
}

void MainWindow::on_btnApply_clicked()
{
    QString theme_name = ui->cmbSvgThemes->currentText();
    
    // Get theme info from our embedded themes
    const ThemeInfo* themeInfo = getThemeInfo(theme_name);
    
    // IMPORTANT: First clear all stylesheets and reset to clean state
    // This prevents themes from leaving persistent colors/styles
    ClearAllStylesheets();
    
    // Force widget updates to ensure clean state before applying new theme
    ForceWidgetUpdate();
    
    // Initialize the resource for the selected theme
    switch (themeInfo->type)
    {
        case ThemeType::Blossom:
            Q_INIT_RESOURCE(Blossom);
            break;
        case ThemeType::Charcoal:
            Q_INIT_RESOURCE(Charcoal);
            break;
        case ThemeType::FTICerise:
            Q_INIT_RESOURCE(FTICerise);
            break;
        case ThemeType::FTICeriseDark:
            Q_INIT_RESOURCE(FTICeriseDark);
            break;
        case ThemeType::FTICobalt:
            Q_INIT_RESOURCE(FTICobalt);
            break;
        case ThemeType::FTICobaltDark:
            Q_INIT_RESOURCE(FTICobaltDark);
            break;
        case ThemeType::FTIMeteorite:
            Q_INIT_RESOURCE(FTIMeteorite);
            break;
        case ThemeType::FTIMeteoriteDark:
            Q_INIT_RESOURCE(FTIMeteoriteDark);
            break;
        case ThemeType::Adapta:
            Q_INIT_RESOURCE(Adapta);
            break;
        case ThemeType::AdaptaNokto:
            Q_INIT_RESOURCE(AdaptaNokto);
            break;
        case ThemeType::KvArc:
            Q_INIT_RESOURCE(KvArc);
            break;
        case ThemeType::KvArcDark:
            Q_INIT_RESOURCE(KvArcDark);
            break;
        case ThemeType::KvGnome:
            Q_INIT_RESOURCE(KvGnome);
            break;
        case ThemeType::KvGnomeDark:
            Q_INIT_RESOURCE(KvGnomeDark);
            break;
        case ThemeType::KvSimplicity:
            Q_INIT_RESOURCE(KvSimplicity);
            break;
        case ThemeType::KvSimplicityDark:
            Q_INIT_RESOURCE(KvSimplicityDark);
            break;
        case ThemeType::KvAmbiance:
            Q_INIT_RESOURCE(KvAmbiance);
            break;
        case ThemeType::KvGnomeAlt:
            Q_INIT_RESOURCE(KvGnomeAlt);
            break;
        case ThemeType::KvOxygen:
            Q_INIT_RESOURCE(KvOxygen);
            break;
        case ThemeType::Retrosmart:
            Q_INIT_RESOURCE(Retrosmart);
            break;
        case ThemeType::Default:
        default:
            Q_INIT_RESOURCE(Default);
            break;
    }

    // Get the current style - with static linking, we can cast directly
    Kvantum::Style* kvantumStyle = qobject_cast<Kvantum::Style*>(qApp->style());
    
    if (kvantumStyle)
    {
        // Use the setTheme method to switch themes
        if (!kvantumStyle->setTheme(themeInfo->configPath, themeInfo->svgPath, themeInfo->colorPath))
        {
            QMessageBox msg(this);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.setText("Failed to load the theme.");
            msg.setWindowTitle("Oopsie!");
            msg.setIcon(QMessageBox::Critical);
            msg.exec();
            return;
        }
        
        // Apply the new palette
        qApp->setPalette(kvantumStyle->standardPalette());
        
        // Update label with current theme
        ui->lblTheme->setText(QString("Current Theme: %1").arg(theme_name));
        
        // Final step: Restyle all windows with the new theme
        RestyleWindow();
    }
    else
    {
        ui->lblTheme->setText("Current style engine is not kvantum.");
    }
}

void MainWindow::on_pushButton_8_clicked()
{
    QMessageBox::question(this, "Test", "Deneme 123",
                                    QMessageBox::Yes|QMessageBox::No);
}
