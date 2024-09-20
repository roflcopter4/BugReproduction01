#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDebug>
#include <QMimeData>
#include <QVBoxLayout>

#ifdef Q_OS_WINDOWS
ND static QStringList getFilePathsViaWin32Idlist(QMimeData const *mimeData);
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      centralWidget(new QWidget(this)),
      textEdit(new QTextEdit(centralWidget))
{
    ui->setupUi(this);
    setAcceptDrops(true);
    setCentralWidget(centralWidget);

    auto *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(textEdit);
    centralWidget->setLayout(layout);

    textEdit->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QMimeData const *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QStringList pathList;

        if (urlList.isEmpty()) {
            qWarning() << u"hasUrls() indicated that file urls should exist, but urls() returned an empty list.";
#ifdef Q_OS_WINDOWS
            pathList = getFilePathsViaWin32Idlist(mimeData);
            if (pathList.isEmpty())
                qWarning() << u"The backup approach failed to determine any file path.";
            else
                qWarning() << u"The backup approach found" << pathList.size() << u"file paths.";
#endif
        } else {
            for (auto const &i : urlList)
                pathList.append(i.toLocalFile());
        }

        for (qsizetype i = 0; i < pathList.size(); ++i) {
            qDebug() << u"File" << i << u":" << pathList[i];
            textEdit->append(u"File " + QString::number(i) + u": " + pathList[i]);
        }
        textEdit->append({});
    }
}

//-----------------------------------------------------------------------------------

#ifdef Q_OS_WINDOWS
# include <ShlObj.h>
# include <Shlwapi.h>
# define GetPIDLFolder(pida)  (reinterpret_cast<LPCITEMIDLIST>((reinterpret_cast<BYTE const *>(pida)) + (pida)->aoffset[0]))
# define GetPIDLItem(pida, i) (reinterpret_cast<LPCITEMIDLIST>((reinterpret_cast<BYTE const *>(pida)) + (pida)->aoffset[(i) + 1]))

static void dumpComError(char16_t const *message, HRESULT res)
{
    qDebug() << u"COM error in" << message << u':'
             << QString::number(static_cast<unsigned>(res), 16) << u':'
             << QString::fromStdString(std::error_code(res, std::system_category()).message());
}

ND static QStringList getFilePathsViaWin32Idlist(QMimeData const *mimeData)
{
    auto shIdList = mimeData->data(uR"(application/x-qt-windows-mime;value="Shell IDList Array")"_s);
    auto idList   = reinterpret_cast<::CIDA const *>(shIdList.data());

    LPWSTR  str = nullptr;
    HRESULT res = ::SHGetNameFromIDList(GetPIDLFolder(idList), SIGDN_DESKTOPABSOLUTEPARSING, &str);
    if (FAILED(res)) {
        dumpComError(u"SHGetNameFromIDList", res);
        return {};
    }
    QString rootDir = QString::fromWCharArray(str);
    rootDir.append(u'\\');
    ::CoTaskMemFree(str);

    QStringList ret;
    ret.reserve(idList->cidl);
    for (unsigned i = 0; i < idList->cidl; ++i) {
        str = nullptr;
        res = ::SHGetNameFromIDList(GetPIDLItem(idList, i), SIGDN_PARENTRELATIVE, &str);
        if (FAILED(res)) {
            dumpComError(u"SHGetNameFromIDList", res);
            continue;
        }
        QString path = rootDir;
        path.append(reinterpret_cast<QChar *>(str), static_cast<qsizetype>(wcslen(str)));
        ::CoTaskMemFree(str);
        ret.push_back(std::move(path));
    }
    return ret;
}

# undef GetPIDLFolder
# undef GetPIDLItem
#endif

