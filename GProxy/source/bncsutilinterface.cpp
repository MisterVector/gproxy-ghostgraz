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

   CODE PORTED FROM THE ORIGINAL GHOST PROJECT: http://ghost.pwner.org/

*/

#include "gproxy.h"
#include "util/Util.h"
#include "bncsutilinterface.h"
#include "bncsutil/bncsutil.h"

//
// CBNCSUtilInterface
//

CBNCSUtilInterface :: CBNCSUtilInterface( string userName, string userPassword )
{
	m_NLS = new NLS( userName, userPassword );
}

CBNCSUtilInterface :: ~CBNCSUtilInterface( )
{
	delete (NLS *)m_NLS;
}

void CBNCSUtilInterface :: Reset( string userName, string userPassword )
{
	delete (NLS *)m_NLS;
	m_NLS = new NLS( userName, userPassword );
}

bool CBNCSUtilInterface :: HELP_SID_AUTH_CHECK( bool TFT, uint32_t war3Version, string war3Path, string keyROC, string keyTFT, string valueStringFormula, string mpqFileName, BYTEARRAY clientToken, BYTEARRAY serverToken )
{
	// set m_EXEVersion, m_EXEVersionHash, m_EXEInfo, m_InfoROC, m_InfoTFT

	string FileWar3EXE = war3Path + "Warcraft III.exe";

	if( !Util::fileExists( FileWar3EXE ) )
		FileWar3EXE = war3Path + "warcraft.exe";

	if( !Util::fileExists( FileWar3EXE ) )
		FileWar3EXE = war3Path + "war3.exe";

	if( !Util::fileExists( FileWar3EXE ) )
		FileWar3EXE = war3Path + "Frozen Throne.exe";

	bool MissingFile = false;

	if( !Util::fileExists( FileWar3EXE ) ) 
	{
		CONSOLE_Print( "[BNCSUI] unable to open [" + QString::fromStdString(FileWar3EXE) + "]" );
		MissingFile = true;
	}

	string FileStormDLL, FileGameDLL;

	if( war3Version <= 28 )
	{
		FileStormDLL = war3Path + "Storm.dll";

		if( !Util::fileExists( FileStormDLL ) )
			FileStormDLL = war3Path + "storm.dll";

		FileGameDLL = war3Path + "Game.dll";

		if( !Util::fileExists( FileGameDLL ) )
		{
			FileGameDLL = war3Path + "game.dll";
		}

		if( !Util::fileExists( FileStormDLL ) )
		{
			CONSOLE_Print( "[BNCSUI] unable to open [" + QString::fromStdString(FileStormDLL) + "]" );
			MissingFile = true;
		}

		if( !Util::fileExists( FileGameDLL ) )
		{
			CONSOLE_Print( "[BNCSUI] unable to open [" + QString::fromStdString(FileGameDLL) + "]" );
			MissingFile = true;
		}
	}

	if( MissingFile )
		return false;

	//todotodo: check getEXEInfo return value to ensure 1024 bytes was enough

	char buf[1024];
	uint32_t EXEVersion;
	getExeInfo( FileWar3EXE.c_str( ), (char *)&buf, 1024, (uint32_t *)&EXEVersion, BNCSUTIL_PLATFORM_X86 );
	m_EXEInfo = buf;
	m_EXEVersion = Util::createByteArray( EXEVersion, false );
	unsigned long EXEVersionHash;

	// for war3version <= 28, we use war3.exe, storm.dll, and game.dll
	// for war3version >= 29, we use Warcraft III.exe only
	if( war3Version <= 28 )
	{
		checkRevisionFlat( valueStringFormula.c_str( ), FileWar3EXE.c_str( ), FileStormDLL.c_str( ), FileGameDLL.c_str( ), extractMPQNumber( mpqFileName.c_str( ) ), (unsigned long *)&EXEVersionHash );
	}
	else
	{
		const char* files[] = { FileWar3EXE.c_str( ) };
		checkRevision( valueStringFormula.c_str( ), files, 1, extractMPQNumber( mpqFileName.c_str( ) ), (unsigned long *)&EXEVersionHash );
	}

	m_EXEVersionHash = Util::createByteArray( (uint32_t) EXEVersionHash, false );
	m_KeyInfoROC = CreateKeyInfo( keyROC, Util::byteArrayToUInt32( clientToken, false ), Util::byteArrayToUInt32( serverToken, false ) );
	
	if( TFT )
		m_KeyInfoTFT = CreateKeyInfo( keyTFT, Util::byteArrayToUInt32( clientToken, false ), Util::byteArrayToUInt32( serverToken, false ) );
			
	if( m_KeyInfoROC.size( ) == 36 && ( !TFT || m_KeyInfoTFT.size( ) == 36 ) )
		return true;
	else
	{
		if( m_KeyInfoROC.size( ) != 36 )
			CONSOLE_Print( ColoredMessage("[BNCSUI] unable to create ROC key info - invalid ROC key" , ColoredMessage::ERROR));

		if( TFT && m_KeyInfoTFT.size( ) != 36 )
			CONSOLE_Print( ColoredMessage("[BNCSUI] unable to create TFT key info - invalid TFT key" , ColoredMessage::ERROR));
	}

	return false;
}

bool CBNCSUtilInterface :: HELP_SID_AUTH_ACCOUNTLOGON( )
{
	// set m_ClientKey

	char buf[32];
	// nls_get_A( (nls_t *)m_nls, buf );
	( (NLS *)m_NLS )->getPublicKey( buf );
	m_ClientKey = Util::createByteArray( (unsigned char *)buf, 32 );
	return true;
}

bool CBNCSUtilInterface :: HELP_SID_AUTH_ACCOUNTLOGONPROOF( BYTEARRAY salt, BYTEARRAY serverKey )
{
	// set m_M1

	char buf[20];
	// nls_get_M1( (nls_t *)m_nls, buf, string( serverKey.begin( ), serverKey.end( ) ).c_str( ), string( salt.begin( ), salt.end( ) ).c_str( ) );
	( (NLS *)m_NLS )->getClientSessionKey( buf, string( salt.begin( ), salt.end( ) ).c_str( ), string( serverKey.begin( ), serverKey.end( ) ).c_str( ) );
	m_M1 = Util::createByteArray( (unsigned char *)buf, 20 );
	return true;
}

bool CBNCSUtilInterface :: HELP_PvPGNPasswordHash( string userPassword )
{
	// set m_PvPGNPasswordHash

	char buf[20];
	hashPassword( userPassword.c_str( ), buf );
	m_PvPGNPasswordHash = Util::createByteArray( (unsigned char *)buf, 20 );
	return true;
}

BYTEARRAY CBNCSUtilInterface :: CreateKeyInfo( string key, uint32_t clientToken, uint32_t serverToken )
{
	unsigned char Zeros[] = { 0, 0, 0, 0 };
	BYTEARRAY KeyInfo;
	CDKeyDecoder Decoder( key.c_str( ), key.size( ) );

	if( Decoder.isKeyValid( ) )
	{
		Util::appendByteArray( KeyInfo, Util::createByteArray( (uint32_t)key.size( ), false ) );
		Util::appendByteArray( KeyInfo, Util::createByteArray( Decoder.getProduct( ), false ) );
		Util::appendByteArray( KeyInfo, Util::createByteArray( Decoder.getVal1( ), false ) );
		Util::appendByteArray( KeyInfo, Util::createByteArray( Zeros, 4 ) );
		size_t Length = Decoder.calculateHash( clientToken, serverToken );
		char *buf = new char[Length];
		Length = Decoder.getHash( buf );
		Util::appendByteArray( KeyInfo, Util::createByteArray( (unsigned char *)buf, Length ) );
		delete [] buf;
	}

	return KeyInfo;
}
