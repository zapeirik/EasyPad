#include <QApplication>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QFont>

const int default_fontsize = 16;
const QString header_text = "EasyPad - v1.0";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle(header_text);

    auto *editor = new QPlainTextEdit(&window);

    QFont font = editor->font();
    font.setPointSize(default_fontsize);
    editor->setFont(font);

    window.setCentralWidget(editor);

    window.resize(600, 200);
    window.show();

    return app.exec();
}
