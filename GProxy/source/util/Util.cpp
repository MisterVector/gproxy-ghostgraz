/*

   Copyright 2010 Trevor Hogan

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include "Util.h"

#include <sys/stat.h>
#include <QByteArray>
#include <QTextCodec>
#include <QRegExp>

/**
 * Hidden constructor.
 * Utility pattern.
 */
Util::Util () { }

BYTEARRAY Util::createByteArray( unsigned char *a, int size )
{
	if( size < 1 )
		return BYTEARRAY( );

	return BYTEARRAY( a, a + size );
}

BYTEARRAY Util::createByteArray( unsigned char c )
{
	BYTEARRAY result;
	result.push_back( c );
	return result;
}

BYTEARRAY Util::createByteArray( uint16_t i, bool reverse )
{
	BYTEARRAY result;
	result.push_back( (unsigned char)i );
	result.push_back( (unsigned char)( i >> 8 ) );

	if( reverse )
		return BYTEARRAY( result.rbegin( ), result.rend( ) );
	else
		return result;
}

BYTEARRAY Util::createByteArray( uint32_t i, bool reverse )
{
	BYTEARRAY result;
	result.push_back( (unsigned char)i );
	result.push_back( (unsigned char)( i >> 8 ) );
	result.push_back( (unsigned char)( i >> 16 ) );
	result.push_back( (unsigned char)( i >> 24 ) );

	if( reverse )
		return BYTEARRAY( result.rbegin( ), result.rend( ) );
	else
		return result;
}

uint16_t Util::byteArrayToUInt16( BYTEARRAY b, bool reverse, unsigned int start )
{
	if( b.size( ) < start + 2 )
		return 0;

	BYTEARRAY temp = BYTEARRAY( b.begin( ) + start, b.begin( ) + start + 2 );

	if( reverse )
		temp = BYTEARRAY( temp.rbegin( ), temp.rend( ) );

	return (uint16_t)( temp[1] << 8 | temp[0] );
}

uint32_t Util::byteArrayToUInt32( BYTEARRAY b, bool reverse, unsigned int start )
{
	if( b.size( ) < start + 4 )
		return 0;

	BYTEARRAY temp = BYTEARRAY( b.begin( ) + start, b.begin( ) + start + 4 );

	if( reverse )
		temp = BYTEARRAY( temp.rbegin( ), temp.rend( ) );

	return (uint32_t)( temp[3] << 24 | temp[2] << 16 | temp[1] << 8 | temp[0] );
}

string Util::byteArrayToDecString( BYTEARRAY b )
{
	if( b.empty( ) )
		return string( );

	string result = Util::toString( b[0] );

	for( BYTEARRAY :: iterator i = b.begin( ) + 1; i != b.end( ); i++ )
		result += " " + Util::toString( *i );

	return result;
}

string Util::byteArrayToHexString( BYTEARRAY b )
{
	if( b.empty( ) )
		return string( );

	string result = Util::toHexString( b[0] );

	for( BYTEARRAY :: iterator i = b.begin( ) + 1; i != b.end( ); i++ )
	{
		if( *i < 16 )
			result += " 0" + Util::toHexString( *i );
		else
			result += " " + Util::toHexString( *i );
	}

	return result;
}

void Util::appendByteArray( BYTEARRAY &b, BYTEARRAY append )
{
	b.insert( b.end( ), append.begin( ), append.end( ) );
}

void Util::appendByteArrayFast( BYTEARRAY &b, BYTEARRAY &append )
{
	b.insert( b.end( ), append.begin( ), append.end( ) );
}

void Util::appendByteArray( BYTEARRAY &b, unsigned char *a, int size )
{
	Util::appendByteArray( b, Util::createByteArray( a, size ) );
}

void Util::appendByteArray( BYTEARRAY &b, string append, bool terminator )
{
	// append the string plus a null terminator

	b.insert( b.end( ), append.begin( ), append.end( ) );

	if( terminator )
		b.push_back( 0 );
}

void Util::appendByteArrayFast( BYTEARRAY &b, string &append, bool terminator )
{
	// append the string plus a null terminator

	b.insert( b.end( ), append.begin( ), append.end( ) );

	if( terminator )
		b.push_back( 0 );
}

void Util::appendByteArray( BYTEARRAY &b, uint16_t i, bool reverse )
{
	Util::appendByteArray( b, Util::createByteArray( i, reverse ) );
}

void Util::appendByteArray( BYTEARRAY &b, uint32_t i, bool reverse )
{
	Util::appendByteArray( b, Util::createByteArray( i, reverse ) );
}

BYTEARRAY Util::extractNTCString( BYTEARRAY &b, unsigned int start )
{
	// start searching the byte array at position 'start' for the first null value
	// if found, return the subarray from 'start' to the null value but not including the null value

	if( start < b.size( ) )
	{
		for( unsigned int i = start; i < b.size( ); i++ )
		{
			if( b[i] == 0 )
				return BYTEARRAY( b.begin( ) + start, b.begin( ) + i );
		}

		// no null value found, return the rest of the byte array

		return BYTEARRAY( b.begin( ) + start, b.end( ) );
	}

	return BYTEARRAY( );
}

QString Util::extractNTString(BYTEARRAY &b, unsigned int start)
{
    if (start < b.size())
    {
        QByteArray encodedString;
        for (unsigned int i = start; i < b.size(); i++)
        {
            if (b[i] == 0)
            {
                return QTextCodec::codecForName("UTF-8")->toUnicode(encodedString);
            }

            encodedString += b.at(i);
        }

        // no null value found, return the rest of the byte array
        return QTextCodec::codecForName("UTF-8")->toUnicode(encodedString);
    }

    return "";
}

QString Util::extractNTString(BYTEARRAY &b, unsigned int start, unsigned int& nextByte)
{
    if (start < b.size())
    {
        QByteArray encodedString;
        for (unsigned int i = start; i < b.size(); ++i)
        {
            if (b[i] == 0)
            {
                nextByte = i + 1;
                return QTextCodec::codecForName("UTF-8")->toUnicode(encodedString);
            }

            encodedString += b.at(i);
        }

        // no null value found, return the rest of the byte array
        nextByte = b.size() + 1;
        return QTextCodec::codecForName("UTF-8")->toUnicode(encodedString);
    }

    return "";
}

QString Util::extractString(BYTEARRAY &b, unsigned int start, unsigned int length)
{
    if (start + length < b.size())
    {
        QByteArray encodedString;
        for (unsigned int i = start; i < start + length; ++i)
        {
            encodedString += b.at(i);
        }

        return QTextCodec::codecForName("UTF-8")->toUnicode(encodedString);
    }

    return "";
}

unsigned char Util::extractHex( BYTEARRAY &b, unsigned int start, bool reverse )
{
	// consider the byte array to contain a 2 character ASCII encoded hex value at b[start] and b[start + 1] e.g. "FF"
	// extract it as a single decoded byte

	if( start + 1 < b.size( ) )
	{
		unsigned int c;
		string temp = string( b.begin( ) + start, b.begin( ) + start + 2 );

		if( reverse )
			temp = string( temp.rend( ), temp.rbegin( ) );

		stringstream SS;
		SS << temp;
		SS >> hex >> c;
		return c;
	}

	return 0;
}

BYTEARRAY Util::extractNumbers( string s, unsigned int count )
{
	// consider the string to contain a bytearray in dec-text form, e.g. "52 99 128 1"

	BYTEARRAY result;
	unsigned int c;
	stringstream SS;
	SS << s;

	for( unsigned int i = 0; i < count; i++ )
	{
		if( SS.eof( ) )
			break;

		SS >> c;

		// todotodo: if c > 255 handle the error instead of truncating

		result.push_back( (unsigned char)c );
	}

	return result;
}

BYTEARRAY Util::extractHexNumbers( string s )
{
	// consider the string to contain a bytearray in hex-text form, e.g. "4e 17 b7 e6"

	BYTEARRAY result;
	unsigned int c;
	stringstream SS;
	SS << s;

	while( !SS.eof( ) )
	{
		SS >> hex >> c;

		// todotodo: if c > 255 handle the error instead of truncating

		result.push_back( (unsigned char)c );
	}

	return result;
}

string Util::toString( unsigned long i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( unsigned short i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( unsigned int i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( long i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( short i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( int i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string Util::toString( float f, int digits )
{
	string result;
	stringstream SS;
	SS << std :: fixed << std :: setprecision( digits ) << f;
	SS >> result;
	return result;
}

string Util::toString( double d, int digits )
{
	string result;
	stringstream SS;
	SS << std :: fixed << std :: setprecision( digits ) << d;
	SS >> result;
	return result;
}

/**
 * Converst a QColor into QString with the format "|caarrggbb". |c is the color start.
 * "aa" is the alpha value which will always be "ff". "rr", "gg" and "bb" are the red, green and blue
 * hexadezimal color values respictively. Examples: red is |cffff0000 and blue is |cff0000ff.
 *
 * @param color The QColor to convert.
 * @return QString
 */
QString Util::toString(const QColor& color)
{
    return "|cff"
            + QString("%1").arg(color.red(), 2, 16, QChar('0'))
            + QString("%1").arg(color.green(), 2, 16, QChar('0'))
            + QString("%1").arg(color.blue(), 2, 16, QChar('0'));
}

string Util::toHexString( uint32_t i )
{
	string result;
	stringstream SS;
	SS << std :: hex << i;
	SS >> result;
	return result;
}

// todotodo: these Util::ToXXX functions don't fail gracefully,
// they just return garbage (in the uint case usually just -1 casted to an unsigned type it looks like)

uint16_t Util::toUInt16( string &s )
{
	uint16_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

uint32_t Util::toUInt32( string &s )
{
	uint32_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int16_t Util::toInt16( string &s )
{
	int16_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int32_t Util::toInt32( string &s )
{
	int32_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

double Util::toDouble( string &s )
{
	double result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

string Util::msToString( uint32_t ms )
{
	string MinString = Util::toString( ( ms / 1000 ) / 60 );
	string SecString = Util::toString( ( ms / 1000 ) % 60 );

	if( MinString.size( ) == 1 )
		MinString.insert( 0, "0" );

	if( SecString.size( ) == 1 )
		SecString.insert( 0, "0" );

	return MinString + "m" + SecString + "s";
}

/**
 * Converst a QString into QColor. The expected format is "|caarrggbb". |c is the start.
 * "aa" is the alpha value. "rr", "gg" and "bb" are the red, green and blue
 * hexadezimal color values respictively. Examples: |cffff0000 is red and |cff0000ff is blue.
 *
 * @param colorString The string containing the color code.
 * @return QColor
 */
QColor Util::toColor (const QString& colorString)
{
    QRegExp colorCodeRegExp = QRegExp("\\|c([a-f]|[0-9]){8}");
    colorCodeRegExp.setCaseSensitivity(Qt::CaseInsensitive);

    bool* ok; // Will be assigned true if the conversation was successful. False otherwise.
    int alpha, red, green, blue; // Color values.

    if (colorString.indexOf(colorCodeRegExp) == 0)
    {
        alpha = colorString.mid(2, 2).toInt(ok, 16);
        red = colorString.mid(4, 2).toInt(ok, 16);
        green = colorString.mid(6, 2).toInt(ok, 16);
        blue = colorString.mid(8, 2).toInt(ok, 16);

        return QColor(red, green, blue, 255);
    }
    else
    {
        return QColor::Invalid;
    }
}

bool Util::fileExists( string file )
{
	struct stat fileinfo;

	if( stat( file.c_str( ), &fileinfo ) == 0 )
		return true;

	return false;
}

string Util::fileRead( string file, uint32_t start, uint32_t length )
{
	ifstream IS;
	IS.open( file.c_str( ), ios :: binary );

	if( IS.fail( ) )
	{
		CONSOLE_Print( "[UTIL] warning - unable to read file part [" + QString::fromStdString(file) + "]" );
		return string( );
	}

	// get length of file

	IS.seekg( 0, ios :: end );
	uint32_t FileLength = IS.tellg( );

	if( start > FileLength )
	{
		IS.close( );
		return string( );
	}

	IS.seekg( start, ios :: beg );

	// read data

	char *Buffer = new char[length];
	IS.read( Buffer, length );
	string BufferString = string( Buffer, IS.gcount( ) );
	IS.close( );
	delete [] Buffer;
	return BufferString;
}

string Util::fileRead( string file )
{
	ifstream IS;
	IS.open( file.c_str( ), ios :: binary );

	if( IS.fail( ) )
	{
		CONSOLE_Print(ColoredMessage("[UTIL] warning - unable to read file [" + QString::fromStdString(file) + "]", ColoredMessage::ERROR));
		return string( );
	}

	// get length of file

	IS.seekg( 0, ios :: end );
	uint32_t FileLength = IS.tellg( );
	IS.seekg( 0, ios :: beg );

	// read data

	char *Buffer = new char[FileLength];
	IS.read( Buffer, FileLength );
	string BufferString = string( Buffer, IS.gcount( ) );
	IS.close( );
	delete [] Buffer;

	if( BufferString.size( ) == FileLength )
		return BufferString;
	else
		return string( );
}

bool Util::fileWrite( string file, unsigned char *data, uint32_t length )
{
	ofstream OS;
	OS.open( file.c_str( ), ios :: binary );

	if( OS.fail( ) )
	{
		CONSOLE_Print(ColoredMessage("[UTIL] warning - unable to write file [" + QString::fromStdString(file) + "]", ColoredMessage::ERROR));
		return false;
	}

	// write data

	OS.write( (const char *)data, length );
	OS.close( );
	return true;
}

string Util::fileSafeName( string fileName )
{
	string :: size_type BadStart = fileName.find_first_of( "\\/:*?<>|" );

	while( BadStart != string :: npos )
	{
		fileName.replace( BadStart, 1, 1, '_' );
		BadStart = fileName.find_first_of( "\\/:*?<>|" );
	}

	return fileName;
}

string Util::addPathSeperator( string path )
{
	if( path.empty( ) )
		return string( );

#ifdef WIN32
	char Seperator = '\\';
#else
	char Seperator = '/';
#endif

	if( *(path.end( ) - 1) == Seperator )
		return path;
	else
		return path + string( 1, Seperator );
}

BYTEARRAY Util::encodeStatString( BYTEARRAY &data )
{
	unsigned char Mask = 1;
	BYTEARRAY Result;

	for( unsigned int i = 0; i < data.size( ); i++ )
	{
		if( ( data[i] % 2 ) == 0 )
			Result.push_back( data[i] + 1 );
		else
		{
			Result.push_back( data[i] );
			Mask |= 1 << ( ( i % 7 ) + 1 );
		}

		if( i % 7 == 6 || i == data.size( ) - 1 )
		{
			Result.insert( Result.end( ) - 1 - ( i % 7 ), Mask );
			Mask = 1;
		}
	}

	return Result;
}

BYTEARRAY Util::decodeStatString( BYTEARRAY &data )
{
	unsigned char Mask = data[0];
	BYTEARRAY Result;

	for( unsigned int i = 0; i < data.size( ); i++ )
	{
		if( ( i % 8 ) == 0 )
			Mask = data[i];
		else
		{
			if( ( Mask & ( 1 << ( i % 8 ) ) ) == 0 )
				Result.push_back( data[i] - 1 );
			else
				Result.push_back( data[i] );
		}
	}

	return Result;
}

QString Util::revertString(const QString& s)
{
    QByteArray bytes = s.toAscii();
    char* data = bytes.data();
    std::reverse(data, data + s.length());
    return QString(data);
}