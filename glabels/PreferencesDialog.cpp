/*  PreferencesDialog.cpp
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

#include "PreferencesDialog.h"
#include "MainWindow.h" 
#include "model/Settings.h"


namespace glabels
{

	///
	/// Constructor
	///
	PreferencesDialog::PreferencesDialog(MainWindow* mainWindow, QWidget* parent )
		: QDialog(parent), m_mainWindow(mainWindow)
	{
		setupUi( this );

		switch ( model::Settings::units().toEnum() )
		{
		case model::Units::IN:
			unitsInchesRadio->setChecked( true );
			break;
		case model::Units::MM:
			unitsMillimetersRadio->setChecked( true );
			break;
		case model::Units::CM:
			unitsCentimetersRadio->setChecked( true );
			break;
		case model::Units::PC:
			unitsPicasRadio->setChecked( true );
			break;
		default:
			unitsPointsRadio->setChecked( true );
			break;
		}

		m_isDarkMode = m_mainWindow->isDarkMode(); //TODO figure out why it doesn't initialize with the correct state

		if (m_isDarkMode == true) {
			themeDarkRadio->setChecked(true);
		} else {
			themeLightRadio->setChecked(true);
		}

	}


	///
	/// Units Radios Changed
	///
	void PreferencesDialog::onUnitsRadiosChanged()
	{
		if ( unitsInchesRadio->isChecked() )
		{
			model::Settings::setUnits( model::Units::in() );
		}
		else if ( unitsMillimetersRadio->isChecked() )
		{
			model::Settings::setUnits( model::Units::mm() );
		}
		else if ( unitsCentimetersRadio->isChecked() )
		{
			model::Settings::setUnits( model::Units::cm() );
		}
		else if ( unitsPicasRadio->isChecked() )
		{
			model::Settings::setUnits( model::Units::pc() );
		}
		else
		{
			model::Settings::setUnits( model::Units::pt() );
		}
	}

    ///
	/// Theme Radios Changed
	///
	void PreferencesDialog::onThemeRadiosChanged()
	{
		m_isDarkMode = themeDarkRadio->isChecked();

		if (m_mainWindow->isDarkMode() == m_isDarkMode)
        return;

		if ( themeLightRadio->isChecked() )
		{
			m_isDarkMode = false;
			m_mainWindow->applyTheme(m_isDarkMode);
		}
		else
		{
			m_isDarkMode = true;
			m_mainWindow->applyTheme(m_isDarkMode);
		}
	}

} // namespace glabels
