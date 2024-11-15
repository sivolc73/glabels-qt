/*  PreferencesDialog.h
 *
 *  Copyright (C) 2016  Jim Evins <evins@snaught.com>
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

#ifndef PreferencesDialog_h
#define PreferencesDialog_h

#include "MainWindow.h"
#include "ui_PreferencesDialog.h"

class MainWindow;

namespace glabels
{

	///
	/// New Label Dialog Widget
	///
	class PreferencesDialog : public QDialog, public Ui_PreferencesDialog
	{
		Q_OBJECT

		/////////////////////////////////
		// Life Cycle
		/////////////////////////////////
	public:
    explicit PreferencesDialog(MainWindow *mainWindow, QWidget *parent = nullptr);


		/////////////////////////////////
		// Slots
		/////////////////////////////////
	private slots:
		void onUnitsRadiosChanged();
		void onThemeRadiosChanged();

	private:
	    bool isDarkMode;
        MainWindow* m_mainWindow;

	};

}


#endif // PreferencesDialog_h
