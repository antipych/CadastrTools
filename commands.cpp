#include "commands.h"

#include "xmlutil.h"

#include <QFileDialog>

#pragma warning(push)
#pragma warning( disable : 4100)
#include <acedads.h>
#include <aced.h>
#include <acutads.h>
#include <acedads.h>
#include <adslib.h>
#include <dbents.h>
#include <dbapserv.h>
#pragma warning(pop)

void fpImportXML()
{
    QString selfilter = "Документ (*.xml)";
    QString fileName = QFileDialog::getOpenFileName(NULL, "Документ кадастровой палаты", QString(), "Документ (*.xml)", &selfilter);
    if(!QFileInfo(fileName).isFile())
        return;

    QFile xmlFile(fileName);
    QDomDocument doc;
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "bad file: " << fileName;
        return;
    }
    if (!doc.setContent(&xmlFile))
    {
        qDebug() << "bad xml file: " << fileName;
        return;
    }
    xmlFile.close();

    AcDbDatabase *pCurrentDB; // pointer to hole current drawing database

    pCurrentDB = acdbHostApplicationServices()->workingDatabase(); // set pointer to workling database
    AcDbBlockTable *pBlockTable;
    pCurrentDB->getBlockTable(pBlockTable, AcDb::kForRead); //get the block table
    AcDbBlockTableRecord *pBlockTableRecord;
    pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite); // get a blocktablerecord
    pBlockTable->close();

    QVector<QDomNode> xmlElms;
    XMLUtil::selectAllNodes(doc.firstChildElement(), "Spatial_Element", xmlElms);


    for(int i = 0; i < xmlElms.size(); ++i)
    {
        QDomNode curElm = xmlElms[i];

        QVector<QDomNode> ords;
        XMLUtil::selectNodes(curElm, "Spelement_Unit/Ordinate", ords);

        Adesk::Boolean closed = Adesk::kFalse;
        if(ords.size() > 1 && XMLUtil::attr(ords.first(), "Num_Geopoint") == XMLUtil::attr(ords.last(), "Num_Geopoint"))
            closed = Adesk::kTrue;

        unsigned int numVertex = ords.size();
        if (closed == Adesk::kTrue)
            --numVertex;

        AcDbPolyline *pPline = new AcDbPolyline(numVertex);
        for(unsigned int j = 0; j < numVertex; ++j)
        {
            QDomNode curOrd = ords[j];

            double x = XMLUtil::attr(curOrd, "X").toDouble();
            double y = XMLUtil::attr(curOrd, "Y").toDouble();

            pPline->addVertexAt(j, AcGePoint2d(x,y));
        }
        pPline->setClosed(closed);

        AcDbObjectId entId;
        pBlockTableRecord->appendAcDbEntity(entId, pPline); // Actually adds the ent to the drawing
        pPline->close();
     }
     pBlockTableRecord->close();
     acedCommand(RTSTR, ACRX_T("_zoom"), RTSTR, ACRX_T("E"));
}
