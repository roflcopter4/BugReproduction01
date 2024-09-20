#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include <QVBoxLayout>

#ifdef Q_OS_WINDOWS
ND static auto getFilePathsViaWin32Idlist   (QMimeData const* mimeData) -> QStringList;
ND static auto getFilePathViaWin32Idlist_alt(QMimeData const* mimeData) -> QString;
ND static auto evilWindowsMimeDataHack      (QMimeData const* mimeData) -> QString;
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
        ret.push_back(path);
    }
    return ret;
}


UU ND static QString getFilePathViaWin32Idlist_alt(QMimeData const *mimeData)
{
    auto shIdList = mimeData->data(uR"(application/x-qt-windows-mime;value="Shell IDList Array")"_s);
    auto idList   = reinterpret_cast<::CIDA const *>(shIdList.data());

    IShellItem *item = nullptr;
    LPWSTR      str  = nullptr;
    HRESULT     res  = ::SHCreateItemFromIDList(GetPIDLFolder(idList), IID_PPV_ARGS(&item));
    if (FAILED(res)) {
        dumpComError(u"SHCreateItemFromIDList", res);
        return {};
    }
    res = item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &str);
    if (FAILED(res)) {
        dumpComError(u"IShellItem::GetDisplayName", res);
        item->Release();
        return {};
    }
    QString path = QString::fromWCharArray(str);
    ::CoTaskMemFree(str);
    item->Release();

    res = ::SHCreateItemFromIDList(GetPIDLItem(idList, 0), IID_PPV_ARGS(&item));
    if (FAILED(res)) {
        dumpComError(u"SHCreateItemFromIDList", res);
        return {};
    }
    res = item->GetDisplayName(SIGDN_PARENTRELATIVE, &str);
    if (FAILED(res)) {
        dumpComError(u"IShellItem::GetDisplayName", res);
        item->Release();
        return {};
    }
    path.append(u'\\');
    path.append(str);
    ::CoTaskMemFree(str);
    item->Release();

    return path;
}

UU ND static QString evilWindowsMimeDataHack(QMimeData const *mimeData)
{
# define IsEvenAlignment(n) static_cast<UINT_PTR>((reinterpret_cast<UINT_PTR>(n) & 1) == 0)

    QByteArray shIdList = mimeData->data(uR"(application/x-qt-windows-mime;value="Shell IDList Array")"_s);
    if (shIdList.size() < 8LL + 35LL + 2LL)
        return {};

    char16_t buffer[512];
    LPCSTR   data = shIdList.constData();
    LPCSTR   end  = data + shIdList.size();
    LPCSTR   last = data + *reinterpret_cast<UINT32 const *>(data + 8);
    data += 35;

    // The drive letter is an ASCII string only. Technically a drive letter can be more
    // than one letter, so we treat it as a string. We use strnlen out of paranoia. It is
    // non-standard but definitely exists on Windows.
    QString str = uR"(\\?\)" + QLatin1StringView(data, static_cast<qsizetype>(strnlen(data, end - data)));

    // We blindly add path separators later so ensure there isn't one.
    if (str.endsWith(u'\\'))
        str.chop(1);
    data += str.size() + 1;
    if (data >= end)
        return {};
    // Skip 28 bytes of unknown data. The data always appears to be aligned to an odd
    // offset. If we're at an even offset, skip 29 instead.
    data += 28 + IsEvenAlignment(data);

    while (data + 2 < end) {
        // ASCII short name (null-terminated). We just skip and ignore.
        data += strnlen(data, end - data) + 1;
        // Skip 46 bytes of unknown data, or 47 if we're aligned.
        data += 46 + IsEvenAlignment(data);
        if (data >= end)
            break;

        // UTF-16 path segment (null-terminated). Qt won't accept an unaligned string,
        // so we must copy it to a buffer first. wcsnlen for paranoia as before.
        SIZE_T    len = wcsnlen(reinterpret_cast<wchar_t const *>(data), end - data);
        char16_t *tmp;
        bool      delTmp;
        if (len < std::size(buffer)) {
            tmp    = buffer;
            delTmp = false;
        } else {
            tmp    = new char16_t[len + 1];
            delTmp = true;
        }
        memcpy(tmp, data, (len + 1) * sizeof(wchar_t));
        str.append(u'\\');
        str.append(reinterpret_cast<QChar *>(tmp), static_cast<qsizetype>(len));
        if (delTmp)
            delete[] tmp;

        // Skip UTF-16 data and null terminator
        data += (len + 1) * sizeof(wchar_t);
        // Skip 16 bytes for each entry but the last, in which case we must skip 18.
        // The `last` offset is 4 bytes forward of where we are.
        data += data + 4 == last ? 18 : 16;
    }

    return str;

# undef IsEvenAlignment
}

# undef GetPIDLFolder
# undef GetPIDLItem
#endif

