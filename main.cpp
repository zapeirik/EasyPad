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
#include <QFileDialog>
#include <QFile>
#include <QSaveFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QCloseEvent>
#include <QTextDocument>
#include <functional>
#include "Config.h"

const int default_fontsize = 16;
const QString header_text = QString(Config::AppName) + " " + QString(Config::AppVersion);
const QString saveFileDialogErrorTitle = "Save failed";

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

static bool saveToFile(QPlainTextEdit *editor, const QString &path, QWidget *parent)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(parent, saveFileDialogErrorTitle, file.errorString());
        return false;
    }
    QTextStream out(&file);
    out << editor->toPlainText();
    if (!file.commit()) {
        QMessageBox::warning(parent, saveFileDialogErrorTitle, file.errorString());
        return false;
    }
    editor->document()->setModified(false);
    return true;
}

// Main window that asks before closing if there are unsaved changes.
class MainWindow : public QMainWindow
{
public:
    std::function<bool()> confirmClose;

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (!confirmClose || confirmClose())
            event->accept();
        else
            event->ignore();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName(Config::AppName);
    app.setApplicationName(Config::AppName);

    MainWindow window;
    window.setWindowTitle(header_text);

    auto *editor = new QPlainTextEdit(&window);
    QString currentPath;

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

    QMenu *fileMenu = window.menuBar()->addMenu("File");
    QAction *saveAs = fileMenu->addAction("Save As…");
    saveAs->setShortcut(QKeySequence::SaveAs);
    QObject::connect(saveAs, &QAction::triggered, &window, [=, &currentPath, &window]() {
        QString path = QFileDialog::getSaveFileName(&window, "Save As", currentPath);
        if (!path.isEmpty() && saveToFile(editor, path, &window)) {
            currentPath = path;
            window.setWindowTitle(QFileInfo(path).fileName() + " - " + header_text);
        }
    });

    QAction *save = fileMenu->addAction("Save");
    save->setShortcut(QKeySequence::Save);   // Cmd+S on macOS, Ctrl+S elsewhere
    QObject::connect(save, &QAction::triggered, &window, [=, &currentPath, &window]() {
    if (currentPath.isEmpty())
        saveAs->trigger();
    else
        saveToFile(editor, currentPath, &window);
    });

    // Returns true if it is OK to close: nothing unsaved, saved, or discarded.
    window.confirmClose = [=, &window]() {
        if (!editor->document()->isModified())
            return true;
        auto answer = QMessageBox::warning(
            &window, "Unsaved changes",
            "Do you want to save your changes before quitting?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);
        if (answer == QMessageBox::Save) {
            save->trigger();
            return !editor->document()->isModified();
        }
        return answer == QMessageBox::Discard;
    };

    window.show();

    return app.exec();
}
