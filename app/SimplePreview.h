/*  SimplePreview.h
 *
 *  Copyright (C) 2013  Jim Evins <evins@snaught.com>
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

#ifndef glabels_SimplePreview_h
#define glabels_SimplePreview_h

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QList>

#include "libglabels/Template.h"


namespace gLabels
{

	class SimplePreview : public QGraphicsView
	{
		Q_OBJECT

	public:
		SimplePreview( QWidget *parent );

		void setTemplate( const libglabels::Template *tmplate );
		void setRotate( bool rotateFlag );

	private:
		void update();
		void clearScene();
		void drawPaper( double pw, double ph );
		void drawLabels();
		void drawLabel( double x, double y, const QPainterPath &path );
		void drawArrow();

		QGraphicsScene             *mScene;
		double                      mScale;
		const libglabels::Template *mTmplate;
		bool                        mRotateFlag;

	};

}

#endif // glabels_SimplePreview_h
