/*  SubstitutionField.h
 *
 *  Copyright (C) 2017  Jim Evins <evins@snaught.com>
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

#ifndef model_SubstitutionField_h
#define model_SubstitutionField_h

#include "Variables.h"

#include "merge/Record.h"

#include <QString>
#include <QStringView>


namespace glabels
{
	namespace model
	{

		class SubstitutionField
		{
		public:
			SubstitutionField();
			SubstitutionField( const QString& string );

			QString evaluate( const merge::Record* record, const Variables* variables ) const;
		
			QString fieldName() const;
			QString defaultValue() const;
			QString format() const;
			QChar   formatType() const;
			bool    newLine() const;

			static bool parse( QStringView& s, SubstitutionField& field );

		private:
			static bool parseFieldName( QStringView& s, SubstitutionField& field );
			static bool parseModifier( QStringView& s, SubstitutionField& field );
			static bool parseDefaultValueModifier( QStringView& s, SubstitutionField& field );
			static bool parseFormatModifier( QStringView& s, SubstitutionField& field );
			static bool parseFormatFlags( QStringView& s, SubstitutionField& field );
			static bool parseFormatWidth( QStringView& s, SubstitutionField& field );
			static bool parseFormatPrecision( QStringView& s, SubstitutionField& field );
			static bool parseFormatType( QStringView& s, SubstitutionField& field );
			static bool parseNaturalInteger( QStringView& s, SubstitutionField& field );
			static bool parseNewLineModifier( QStringView& s, SubstitutionField& field );

			QString formatValue( const QString& value ) const;

			QString mFieldName;

			QString mDefaultValue;

			QString mFormat;
			QChar   mFormatType;

			bool    mNewLine;
		};

	}
}


#endif // model_SubstitutionField_h
