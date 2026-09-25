#include <QApplication>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QFont>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QFormLayout>
#include <QFontComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QKeySequence>
#include <QSettings>
#include "Config.h"

const int default_fontsize = 16;
const QString header_text = QString(Config::AppName) + " " + QString(Config::AppVersion);

// Opens a modal Settings dialog for choosing font family and size.
// Changes preview live; on OK they are applied and saved via QSettings.
static void openSettingsDialog(QPlainTextEdit *editor, QWidget *parent)
{
    const QFont original = editor->font();
 
    QDialog dialog(parent);
    dialog.setWindowTitle("Settings");
 
    auto *fontBox = new QFontComboBox(&dialog);
    fontBox->setCurrentFont(original);
 
    auto *sizeBox = new QSpinBox(&dialog);
    sizeBox->setRange(6, 96);
    sizeBox->setValue(original.pointSize() > 0 ? original.pointSize() : 12);
 
    auto *form = new QFormLayout(&dialog);
    form->addRow("Font", fontBox);
    form->addRow("Size", sizeBox);
 
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form->addRow(buttons);
 
    // Live preview while the dialog is open.
    auto preview = [editor, fontBox, sizeBox]() {
        QFont f = fontBox->currentFont();
        f.setPointSize(sizeBox->value());
        editor->setFont(f);
    };
    QObject::connect(fontBox, &QFontComboBox::currentFontChanged, &dialog, preview);
    QObject::connect(sizeBox, &QSpinBox::valueChanged, &dialog, preview);
 
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
 
    if (dialog.exec() == QDialog::Accepted) {
        QFont chosen = fontBox->currentFont();
        chosen.setPointSize(sizeBox->value());
        editor->setFont(chosen);
 
        QSettings settings;
        settings.setValue("font/family", chosen.family());
        settings.setValue("font/size", chosen.pointSize());
    } else {
        editor->setFont(original);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(Config::AppName);
    app.setApplicationName(Config::AppName);

    QMainWindow window;
    window.setWindowTitle(header_text);

    auto *editor = new QPlainTextEdit(&window);

    window.setCentralWidget(editor);
    window.resize(Config::DefaultWidth, Config::DefaultHeight);

    // Load font
    QSettings settings;
    QFont font = editor->font();
    font.setFamily(settings.value("font/family", font.family()).toString());
    font.setPointSize(settings.value("font/size", default_fontsize).toInt());
    editor->setFont(font);

    QMenu *appMenu = window.menuBar()->addMenu(Config::AppName);
    QAction *prefs = appMenu->addAction("Settings…");
    prefs->setMenuRole(QAction::PreferencesRole);
    prefs->setShortcut(QKeySequence::Preferences);
    QObject::connect(prefs, &QAction::triggered, &window, [editor, &window]() {
        openSettingsDialog(editor, &window);
    });

    window.show();

    return app.exec();
}
