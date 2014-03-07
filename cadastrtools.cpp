#include "cadastrtools.h"

#pragma warning(push)
#pragma warning( disable : 4100)
#include <rxdlinkr.h>
#include <acutads.h>
#include <accmd.h>
#include <rxclass.h>

#include <aced.h>
#include <acutads.h>
#include <acedads.h>
#include <adslib.h>
#include <dbents.h>
#include <dbapserv.h>
#pragma warning(pop)

#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#error _DEBUG should not be defined except in internal Adesk debug builds
#endif

#include <QtPlugin>
#ifdef STATIC_BUILD
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#include <QApplication>
#include <QFileInfo>
#include <QDir>

#include "commands.h"

QApplication * theQAppPtr = NULL;
int argc = 0;

HINSTANCE _hdllInstance = 0;

namespace CadastrTools {
    QString path;
    QString logFileName;

    QTextStream& getLog()
    {
        static QFile logFile(logFileName);
        if(!logFile.isOpen())
        {
            logFile.open(QIODevice::WriteOnly | QIODevice::Text);
        }
        static QTextStream l(&logFile);
        return l;
    }

    void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        switch (type) {
        case QtDebugMsg:
            getLog() << QString("Debug: %1 (%2)\n").arg(msg, QString(context.function));
            break;
        case QtWarningMsg:
            getLog() << QString("Warning: %1 (%2)\n").arg(msg, QString(context.function));
            break;
        case QtCriticalMsg:
            getLog() << QString("Critical: %1 (%2)\n").arg(msg, QString(context.function));
            break;
        case QtFatalMsg:
            getLog() << QString("Fatal: %1 (%2)\n").arg(msg, QString(context.function));
        }
        getLog().flush();
    }
}

const QString& CadastrTools::getPath()
{
    return path;
}

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
    _hdllInstance = hInstance;
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _hdllInstance =  hInstance;
    }

    return 1;
}

// Initialization function called in acrxEntryPoint during
// the kInitAppMsg case.  This is where commands are added
// to the AcEd command stack.
//
void
initApp()
{
    ACHAR buf[MAX_PATH] = { 0 };

    GetModuleFileName(_hdllInstance, buf, 1024);

    CadastrTools::path        = QFileInfo(QString::fromWCharArray(buf)).canonicalPath();
    CadastrTools::logFileName = QDir::tempPath() + "/cadastrtools.log";
    qInstallMessageHandler(CadastrTools::MessageHandler);

    qDebug() << "start init";

    theQAppPtr = new QApplication(argc, 0);
    theQAppPtr->setOrganizationName("Farpoint");
    theQAppPtr->setApplicationName("Cadastr Tools");
    theQAppPtr->setOrganizationDomain("farpoint-nn.ru");

    acedRegCmds->addCommand(ACRX_T("FP_CADASTR_COMMANDS"),
        ACRX_T("FPIMPORTXML"), ACRX_T("FPIMPORTXML"), ACRX_CMD_TRANSPARENT, fpImportXML);

    qDebug() << "init ok";

    QString cuixFileName = CadastrTools::path + "/CadastrTools.cuix";
    if(QFile::exists(cuixFileName)) {
        ads_queueexpr(L"(command \"_.cuiunload\" \"CadastrTools\") ");
        ads_queueexpr(L"(command \"FILEDIA\" \"0\") ");

        QString cmd = "(command \"_.cuiload\" \"" + cuixFileName + "\") ";
        ACHAR cmdBuf[MAX_PATH + 256] = { 0 };
        cmd.toWCharArray(cmdBuf);

        ads_queueexpr(cmdBuf);
        ads_queueexpr(L"(command \"FILEDIA\" \"1\") ");
    }
}

// Clean up function called in acrxEntryPoint during the
// kUnloadAppMsg case.  This app's command group is removed
// from the AcEd command stack.
//
void
unloadApp()
{
    acedRegCmds->removeGroup(ACRX_T("FP_CADASTR_COMMANDS"));
    delete theQAppPtr;
}

// ARX entry point.
//
extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(appId);
        acrxDynamicLinker->registerAppMDIAware(appId);
        initApp();
        break;
    case AcRx::kUnloadAppMsg:
        unloadApp();
    }
    return AcRx::kRetOK;
}
