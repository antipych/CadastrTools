#include "xmlutil.h"

QVariant XMLUtil::attr(const QDomNode &node, const QString &name)
{
    QDomNode n = node.attributes().namedItem(name);
    if(n.isNull())
    {
        return QVariant();
    }
    return n.nodeValue();
}

bool XMLUtil::setAttr(QDomNode n, const QString &name, const QVariant &value)
{
    QDomNode attr = n.attributes().namedItem(name);
    if(attr.isNull())
    {
        attr = n.ownerDocument().createAttribute(name);
        n.attributes().setNamedItem(attr);
    }
    if(attr.nodeValue() != value.toString())
    {
        attr.setNodeValue(value.toString());
        return true;
    }
    return false;
}

QDomNode XMLUtil::selectSingleNode(const QDomNode root, const QString xpath)
{
    QVector<QDomNode> out;
    if(selectNodes(root, xpath, out, true))
    {
          return out.first();
    }
    return QDomNode();
}

bool XMLUtil::selectNodes(const QDomNode root, const QString xpath, QVector<QDomNode>& out, bool single)
{
    bool result = false;
    if(xpath.at(0) == "/")
    {
        // from root
        return selectNodes(root.ownerDocument(), xpath.section('/', 1), out, single);
    }

    bool cnt = xpath.contains('/');
    QString step = cnt ? xpath.section('/', 0, 0) : xpath;

    QStringList names = step.split('|');

    QRegExp rx("\\[(.+)\\]");
    QRegExp rxf("@(.+)='(.+)'");

    QStringList nodeNames;

    struct XPathFilter
    {
        QString attrName;
        QString attrValue;
    };

    typedef std::pair <QString, XPathFilter> filter_pair;
    typedef std::multimap<QString, XPathFilter> filter_map;
    typedef std::multimap<QString, XPathFilter>::const_iterator filter_iter;
    typedef std::pair<typename filter_iter, typename filter_iter> filter_iter_pair;

    std::multimap<QString, XPathFilter> nodeFilters;

    for(int i = 0; i < names.size(); ++i)
    {
        QString name = names[i];
        int pos = rx.indexIn(name);
        if (pos > -1)
        {
            QString cname = name.replace(rx, "");
            nodeNames.append(cname);

            int fpos = rxf.indexIn(rx.cap(1));
            if (fpos > -1)
            {
                XPathFilter f;
                f.attrName  = rxf.cap(1);
                f.attrValue = rxf.cap(2);

                nodeFilters.insert(filter_pair(cname, f));
            }
        }
        else
        {
            nodeNames.append(name);
        }
    }

    bool anyName = nodeNames.contains("*");

    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode n = childNodes.at(i);
        QString nodeName = n.nodeName();

        if(anyName || nodeNames.contains(nodeName))
        {
            bool filterPass = true;
            filter_iter_pair p = anyName ? nodeFilters.equal_range("*") : nodeFilters.equal_range(nodeName);
            for(filter_iter fi = p.first; fi != p.second; ++fi)
            {
                if(n.attributes().namedItem(fi->second.attrName).nodeValue() != fi->second.attrValue)
                {
                    filterPass = false;
                    break;
                }
            }

            if(!filterPass) continue;

            if(cnt)
            {
                result = selectNodes(n, xpath.section('/', 1), out, single);
            }
            else
            {
                out.push_back(n);
                result = true;
            }

            if(single && result) break;
        }
    }
    return result;
}

void XMLUtil::selectAllNodes(const QDomNode root, const QString nodeName, QVector<QDomNode> &out)
{
    QDomNodeList childNodes = root.childNodes();
    for(int i = 0; i < childNodes.count(); ++i)
    {
        QDomNode n = childNodes.at(i);
        if(n.nodeName() == nodeName) {
            out.push_back(n);
        }
        else {
            selectAllNodes(n, nodeName, out);
        }
    }
}
