#ifndef CPLAN_H
#define CPLAN_H

namespace XMLUtil
{
    QVariant attr(const QDomNode& n, const QString& name);
    bool     setAttr(QDomNode n, const QString& name, const QVariant& value);

    QDomNode selectSingleNode(const QDomNode root, const QString xpath);
    bool     selectNodes(const QDomNode root, const QString xpath, QVector<QDomNode> &out, bool single = false);
    void     selectAllNodes(const QDomNode root, const QString nodeName, QVector<QDomNode> &out);
}

#endif // CPLAN_H
