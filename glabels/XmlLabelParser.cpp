/*  XmlLabelParser.cpp
 *
 *  Copyright (C) 2014-2016  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "XmlLabelParser.h"

#include "LabelModel.h"
#include "LabelModelObject.h"
#include "LabelModelBoxObject.h"
#include "LabelModelEllipseObject.h"
//#include "LabelObjectLine.h"
//#include "LabelObjectImage.h"
//#include "LabelObjectBarcode.h"
#include "Merge/Factory.h"
#include "libglabels/XmlTemplateParser.h"
#include "libglabels/XmlUtil.h"

#include <QFile>
#include <QByteArray>
#include <zlib.h>
#include <QtDebug>


LabelModel*
XmlLabelParser::readFile( const QString& fileName )
{
	QFile file( fileName );

	if ( !file.open( QFile::ReadOnly ) )
	{
		qWarning() << "Error: Cannot read file " << qPrintable(fileName)
			   << ": " << file.errorString();
		return 0;
	}

	QDomDocument doc;
	bool         success;
	QString      errorString;
	int          errorLine;
	int          errorColumn;

	QByteArray rawData = file.readAll();
	if ( ((rawData[0]&0xFF) == 0x1F) && ((rawData[1]&0xFF) == 0x8b) ) // gzip magic number 0x1F, 0x8B
	{
		// gzip compressed format
		QByteArray unzippedData;
		gunzip( rawData, unzippedData );
		success = doc.setContent( unzippedData, false, &errorString, &errorLine, &errorColumn );
	}
	else
	{
		// plain text
		success = doc.setContent( rawData, false, &errorString, &errorLine, &errorColumn );
	}

	if ( !success )
	{
		qWarning() << "Error: Parse error at line " << errorLine
			   << "column " << errorColumn
			   << ": " << errorString;
		return 0;
	}
	

	QDomElement root = doc.documentElement();
	if ( root.tagName() != "Glabels-document" )
	{
		qWarning() << "Error: Not a Glabels-document file";
		return 0;
	}

	return parseRootNode( root );
}


LabelModel*
XmlLabelParser::readBuffer( const QByteArray& buffer )
{
	QDomDocument doc;
	QString      errorString;
	int          errorLine;
	int          errorColumn;

	if ( !doc.setContent( buffer, false, &errorString, &errorLine, &errorColumn ) )
	{
		qWarning() << "Error: Parse error at line " << errorLine
			   << "column " << errorColumn
			   << ": " << errorString;
		return 0;
	}

	QDomElement root = doc.documentElement();
	if ( root.tagName() != "Glabels-document" )
	{
		qWarning() << "Error: Not a Glabels-document file";
		return 0;
	}

	return parseRootNode( root );
}


QList<LabelModelObject*>
XmlLabelParser::deserializeObjects( const QByteArray& buffer )
{
	QList<LabelModelObject*> list;
	
	QDomDocument doc;
	QString      errorString;
	int          errorLine;
	int          errorColumn;

	if ( !doc.setContent( buffer, false, &errorString, &errorLine, &errorColumn ) )
	{
		qWarning() << "Error: Parse error at line " << errorLine
			   << "column " << errorColumn
			   << ": " << errorString;
		return list;
	}

	QDomElement root = doc.documentElement();
	if ( root.tagName() != "Glabels-objects" )
	{
		qWarning() << "Error: Not a Glabels-objects stream";
		return list;
	}

	return parseObjects( root );
}


void
XmlLabelParser::gunzip( const QByteArray& data, QByteArray& result )
{
        result.clear();

	if (data.size() <= 4) {
		qWarning("XmlLabelParser::gunzip: Input data is truncated");
		return;
	}

	// setup stream for inflate()
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = data.size();
	strm.next_in = (Bytef*)(data.data());

	int ret = inflateInit2(&strm, MAX_WBITS + 16); // gzip decoding
	if (ret != Z_OK)
	{
		return;
	}

	static const int CHUNK_SIZE = 1024;
	char out[CHUNK_SIZE];

	// run inflate(), one chunk at a time
	do {
		strm.avail_out = CHUNK_SIZE;
		strm.next_out = (Bytef*)(out);

		ret = inflate(&strm, Z_NO_FLUSH);
		Q_ASSERT(ret != Z_STREAM_ERROR);  // state not clobbered

		if ( (ret == Z_NEED_DICT) || (ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR) )
		{
			// clean up
			inflateEnd(&strm);
			return;
		}

		result.append(out, CHUNK_SIZE - strm.avail_out);
	} while (strm.avail_out == 0);

	// clean up
	inflateEnd(&strm);
}


LabelModel*
XmlLabelParser::parseRootNode( const QDomElement &node )
{
	using namespace glabels;

	LabelModel* label = new LabelModel();

	/* Pass 1, extract data nodes to pre-load cache. */
	for ( QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling() )
	{
		if ( child.toElement().tagName() == "Data" )
		{
			parseDataNode( child.toElement(), label );
		}
	}

	/* Pass 2, now extract everything else. */
	for ( QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling() )
	{
		QString tagName = child.toElement().tagName();
		
		if ( tagName == "Template" )
		{
			Template* tmplate = XmlTemplateParser().parseTemplateNode( child.toElement() );
			if ( tmplate == 0 )
			{
				qWarning() << "Unable to parse template";
				return 0;
			}
			label->setTmplate( tmplate );
		}
		else if ( tagName == "Objects" )
		{
			parseObjectsNode( child.toElement(), label );
		}
		else if ( tagName == "Merge" )
		{
			parseMergeNode( child.toElement(), label );
		}
		else if ( tagName == "Data" )
		{
			/* Handled in pass 1. */
		}
		else if ( !child.isComment() )
		{
			qWarning() << "Unexpected" << node.tagName() << "child:" << tagName;
		}
	}

	label->clearModified();
	return label;
}


QList<LabelModelObject*>
XmlLabelParser::parseObjects( const QDomElement &node )
{
	QList<LabelModelObject*> list;

	for ( QDomNode child = node.firstChild(); !child.isNull(); child = child.nextSibling() )
	{
		QString tagName = child.toElement().tagName();
		
		if ( tagName == "Object-box" )
		{
			list.append( parseObjectBoxNode( child.toElement() ) );
		}
		else if ( tagName == "Object-ellipse" )
		{
			list.append( parseObjectEllipseNode( child.toElement() ) );
		}
#if 0
		else if ( tagName == "Object-line" )
		{
			list.append( parseObjectLineNode( child.toElement() ) );
		}
		else if ( tagName == "Object-image" )
		{
			list.append( parseObjectImageNode( child.toElement() ) );
		}
		else if ( tagName == "Object-barcode" )
		{
			list.append( parseObjectBarcodeNode( child.toElement() ) );
		}
		else if ( tagName == "Object-text" )
		{
			list.append( parseObjectTextNode( child.toElement() ) );
		}
#endif
		else if ( !child.isComment() )
		{
			qWarning() << "Unexpected" << node.tagName() << "child:" << tagName;
		}
	}

	return list;
}


void
XmlLabelParser::parseObjectsNode( const QDomElement &node, LabelModel* label )
{
	QList<LabelModelObject*> list = parseObjects( node );

	foreach ( LabelModelObject* object, list )
	{
		label->addObject( object );
	}
}


LabelModelBoxObject*
XmlLabelParser::parseObjectBoxNode( const QDomElement &node )
{
	using namespace glabels;

	LabelModelBoxObject* object = new LabelModelBoxObject();


	/* position attrs */
	object->setX0( XmlUtil::getLengthAttr( node, "x", 0.0 ) );
	object->setY0( XmlUtil::getLengthAttr( node, "y", 0.0 ) );

	/* size attrs */
	object->setW( XmlUtil::getLengthAttr( node, "w", 0 ) );
	object->setH( XmlUtil::getLengthAttr( node, "h", 0 ) );

	/* line attrs */
	object->setLineWidth( XmlUtil::getLengthAttr( node, "line_width", 1.0 ) );
        {
		QString  key        = XmlUtil::getStringAttr( node, "line_color_field", "" );
		bool     field_flag = !key.isEmpty();
		uint32_t color      = XmlUtil::getUIntAttr( node, "line_color", 0 );

		object->setLineColorNode( ColorNode( field_flag, color, key ) );
	}

	/* fill attrs */
	{
		QString  key        = XmlUtil::getStringAttr( node, "line_color_field", "" );
		bool     field_flag = !key.isEmpty();
		uint32_t color      = XmlUtil::getUIntAttr( node, "fill_color", 0 );

		object->setFillColorNode( ColorNode( field_flag, color, key ) );
	}
        
	/* affine attrs */
	parseAffineAttrs( node, object );

	/* shadow attrs */
	parseShadowAttrs( node, object );

	return object;
}


LabelModelEllipseObject*
XmlLabelParser::parseObjectEllipseNode( const QDomElement &node )
{
	using namespace glabels;

	LabelModelEllipseObject* object = new LabelModelEllipseObject();


	/* position attrs */
	object->setX0( XmlUtil::getLengthAttr( node, "x", 0.0 ) );
	object->setY0( XmlUtil::getLengthAttr( node, "y", 0.0 ) );

	/* size attrs */
	object->setW( XmlUtil::getLengthAttr( node, "w", 0 ) );
	object->setH( XmlUtil::getLengthAttr( node, "h", 0 ) );

	/* line attrs */
	object->setLineWidth( XmlUtil::getLengthAttr( node, "line_width", 1.0 ) );
        {
		QString  key        = XmlUtil::getStringAttr( node, "line_color_field", "" );
		bool     field_flag = !key.isEmpty();
		uint32_t color      = XmlUtil::getUIntAttr( node, "line_color", 0 );

		object->setLineColorNode( ColorNode( field_flag, color, key ) );
	}

	/* fill attrs */
	{
		QString  key        = XmlUtil::getStringAttr( node, "line_color_field", "" );
		bool     field_flag = !key.isEmpty();
		uint32_t color      = XmlUtil::getUIntAttr( node, "fill_color", 0 );

		object->setFillColorNode( ColorNode( field_flag, color, key ) );
	}
        
	/* affine attrs */
	parseAffineAttrs( node, object );

	/* shadow attrs */
	parseShadowAttrs( node, object );

	return object;
}


LabelModelLineObject*
XmlLabelParser::parseObjectLineNode( const QDomElement &node )
{
	return 0;
}


LabelModelImageObject*
XmlLabelParser::parseObjectImageNode( const QDomElement &node )
{
	return 0;
}


LabelModelBarcodeObject*
XmlLabelParser::parseObjectBarcodeNode( const QDomElement &node )
{
	return 0;
}


LabelModelTextObject*
XmlLabelParser::parseObjectTextNode( const QDomElement &node )
{
	return 0;
}


void
XmlLabelParser::parseTopLevelSpanNode( const QDomElement &node, LabelModelTextObject* object )
{
}


void
XmlLabelParser::parseAffineAttrs( const QDomElement &node, LabelModelObject* object )
{
	using namespace glabels;

	double a[6];

	a[0] = XmlUtil::getDoubleAttr( node, "a0", 0.0 );
	a[1] = XmlUtil::getDoubleAttr( node, "a1", 0.0 );
	a[2] = XmlUtil::getDoubleAttr( node, "a2", 0.0 );
	a[3] = XmlUtil::getDoubleAttr( node, "a3", 0.0 );
	a[4] = XmlUtil::getDoubleAttr( node, "a4", 0.0 );
	a[5] = XmlUtil::getDoubleAttr( node, "a5", 0.0 );

	object->setMatrix( QMatrix( a[0], a[1], a[2], a[3], a[4], a[5] ) );
}


void
XmlLabelParser::parseShadowAttrs( const QDomElement &node, LabelModelObject* object )
{
	using namespace glabels;

	object->setShadow( XmlUtil::getBoolAttr( node, "shadow", false ) );

	if ( object->shadow() )
	{
		object->setShadowX( XmlUtil::getLengthAttr( node, "shadow_x", 0.0 ) );
		object->setShadowY( XmlUtil::getLengthAttr( node, "shadow_y", 0.0 ) );

		QString  key        = XmlUtil::getStringAttr( node, "shadow_color_field", "" );
		bool     field_flag = !key.isEmpty();
		uint32_t color      = XmlUtil::getUIntAttr( node, "shadow_color", 0 );

		object->setShadowColorNode( ColorNode( field_flag, color, key ) );

		object->setShadowOpacity( XmlUtil::getDoubleAttr( node, "shadow_opacity", 1.0 ) );
	}
}


void
XmlLabelParser::parseMergeNode( const QDomElement &node, LabelModel* label )
{
	using namespace glabels;

	QString type = XmlUtil::getStringAttr( node, "type", "None" );
	QString src  = XmlUtil::getStringAttr( node, "src", "" );

	merge::Merge* merge = merge::Factory::createMerge( type );
	merge->setSource( src );

	label->setMerge( merge );
}


void
XmlLabelParser::parseDataNode( const QDomElement &node, LabelModel* label )
{
}


void
XmlLabelParser::parsePixdataNode( const QDomElement &node, LabelModel* label )
{
}


void
XmlLabelParser::parseFileNode( const QDomElement &node, LabelModel* label )
{
}


