/*
 * Copyright (C) Andrea Diamantini 2020 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "settingsdialog.h"
#include "ui_settings.h"

#include <QColorDialog>
#include <QDebug>
#include <QFontDialog>
#include <QMessageBox>
#include <QSettings>

#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/Theme>


SettingsDialog::SettingsDialog(QWidget *parent) 
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Cutepad Settings");

    ui->spacesSpinBox->setRange(1,12);
        
    connect(ui->lineColorButton, &QPushButton::clicked, this, &SettingsDialog::chooseHighlightColor);
    connect(ui->fontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);
    connect(ui->resetButton, &QPushButton::clicked, this, &SettingsDialog::resetSettings);

    loadSettings();

    connect(ui->lineNumbersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::saveSettings);
    connect(ui->highlightCurrentLineCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::saveSettings);

    connect(ui->replaceTabsWithSpacesCheckBox, &QCheckBox::stateChanged, this, &SettingsDialog::saveSettings);
    connect(ui->spacesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::saveSettings);

    connect(ui->syntaxHighlightingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::saveSettings);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::loadSettings()
{
    // the settings object
    QSettings s;

    int lineNumbersMode = s.value("LineNumbers", 0).toInt();
    ui->lineNumbersComboBox->setCurrentIndex(lineNumbersMode);

    bool highlight = s.value("CurrentLineHighlight", false).toBool();
    ui->highlightCurrentLineCheckBox->setChecked(highlight);
    
    QColor highlightLineColor = s.value("HighlightLineColor", QColor(Qt::yellow).lighter(160)).value<QColor>();
    QPalette p = ui->lineColorButton->palette();
    p.setColor(QPalette::Button, highlightLineColor);
    ui->lineColorButton->setPalette(p);

    bool tabReplace = s.value("TabReplace", false).toBool();
    ui->replaceTabsWithSpacesCheckBox->setChecked(tabReplace);

    int tabsCount = s.value("TabsCount", 4).toInt();
    ui->spacesSpinBox->setValue(tabsCount);
    
    // font
    QString fontFamily = s.value("fontFamily", "Monospace").toString();
    int fontSize = s.value("fontSize", 12).toInt();
    int fontWeight = s.value("fontWeight", 50).toInt();
    bool italic = s.value("fontItalic", false).toBool();
    QFont font(fontFamily,fontSize, fontWeight);
    font.setItalic(italic);
    QString fontName = fontFamily + ", " + QString::number(fontSize) + "pt";
    ui->fontLabel->setText(fontName);
    ui->fontLabel->setFont(font);

    // syntax highlight
    KSyntaxHighlighting::Repository repo;
    QVector<KSyntaxHighlighting::Theme> themes = repo.themes();
    QStringList themeNames;
    themeNames << "No Theme";
    for(int i = 0; i < themes.size(); i++) {
        QString n = themes.at(i).name();
        themeNames << n;
    }
    ui->syntaxHighlightingComboBox->addItems(themeNames);
    QString sht = s.value("SyntaxHightlightTheme", "Breeze Light").toString();
    int index = themeNames.indexOf(sht);
    if (index == -1) {
        index = 0;
    }
    ui->syntaxHighlightingComboBox->setCurrentIndex(index);
}


void SettingsDialog::saveSettings()
{
    // the settings object
    QSettings s;

    int lineNumbersMode = ui->lineNumbersComboBox->currentIndex();
    s.setValue("LineNumbers", lineNumbersMode);

    bool highlight = ui->highlightCurrentLineCheckBox->isChecked();
    s.setValue("CurrentLineHighlight", highlight);
    
    QPalette p = ui->lineColorButton->palette();
    QColor highlightLineColor = p.color(QPalette::Button);
    s.setValue("HighlightLineColor", highlightLineColor);
    
    bool tabReplace = ui->replaceTabsWithSpacesCheckBox->isChecked();
    s.setValue("TabReplace", tabReplace);
    
    int tabsCount = ui->spacesSpinBox->value();
    s.setValue("TabsCount",tabsCount);

    // font
    QFont f = ui->fontLabel->font();
    QString fontFamily = f.family();
    int fontSize = f.pointSize();
    int fontWeight = f.weight();
    bool italic = f.italic();
    s.setValue("fontFamily", fontFamily);
    s.setValue("fontSize", fontSize);
    s.setValue("fontWeight", fontWeight);
    s.setValue("fontItalic", italic);

    // syntax highlight
    QString name = ui->syntaxHighlightingComboBox->currentText();
    s.setValue("SyntaxHightlightTheme", name);
}


void SettingsDialog::chooseHighlightColor()
{
    QPalette p = ui->lineColorButton->palette();
    QColor initialColor = p.color(QPalette::Button);
    
    QColor chosenColor = QColorDialog::getColor(initialColor, this, "Choose Highlight Line Color");
    if (chosenColor.isValid()) {
        p.setColor(QPalette::Button, chosenColor);
        ui->lineColorButton->setPalette(p);
        
        saveSettings();
    }
}


void SettingsDialog::chooseFont()
{
    bool ok;
    // initialFont is the font initially shown in the dialog
    QFont initialFont = ui->fontLabel->font();
    QFont font = QFontDialog::getFont(&ok, initialFont, this, "Select font", QFontDialog::MonospacedFonts);
    if (!ok) {
        return;
    }

    QString fontName = font.family() + ", " + QString::number(font.pointSize()) + "pt";
    ui->fontLabel->setText(fontName);
    ui->fontLabel->setFont(font);

    saveSettings();
}


void SettingsDialog::resetSettings()
{
    int risp = QMessageBox::question(this,
                                     "Reset All Settings",
                                     "Are you sure you want to reset all settings?",
                                     QMessageBox::Reset | QMessageBox::Cancel);

    switch(risp) {
        case QMessageBox::Reset: {

            // the settings object
            QSettings s;
            s.clear();
            s.sync();
            loadSettings();
            break;
        }
        case QMessageBox::Cancel:
            return;

        default:
            // this should never happen
            break;
    }
}
