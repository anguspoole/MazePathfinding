#include "cNameValuePair.h"

void cNameValuePair::Reset(void)
{
	this->fvec4Value.x = this->fvec4Value.y = this->fvec4Value.z = this->fvec4Value.w = 0.0f;
	this->sValue = "";
	this->iValue = 0;
	return;
}

cNameValuePair::cNameValuePair()
{
	this->Reset();
	return;
}

cNameValuePair::cNameValuePair( const std::string &name )
{
	this->Reset();
	this->name = name;
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, fvec4 fvec4Value )
{
	this->Reset();
	this->name = name;
	this->fvec4Value = fvec4Value;
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, float fValue )
{
	this->Reset();
	this->name = name;
	this->fvec4Value.x = fValue;
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, int iValue )
{
	this->Reset();
	this->name = name;
	this->iValue = iValue;
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, std::string sValue )
{
	this->Reset();
	this->name = name;
	this->sValue = sValue;
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair childNVP3 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair &childNVP3, const cNameValuePair &childNVP4 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
				const cNameValuePair &childNVP6 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
				const cNameValuePair &childNVP6, const cNameValuePair &childNVP7 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	this->vecChildNVPairs.push_back( childNVP7 );
	return;
}

cNameValuePair::cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		        const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
				const cNameValuePair &childNVP6, const cNameValuePair &childNVP7, const cNameValuePair &childNVP8 )
{
	this->Reset();
	this->vecChildNVPairs.push_back( childNVP0 );
	this->vecChildNVPairs.push_back( childNVP1 );
	this->vecChildNVPairs.push_back( childNVP2 );
	this->vecChildNVPairs.push_back( childNVP3 );
	this->vecChildNVPairs.push_back( childNVP4 );
	this->vecChildNVPairs.push_back( childNVP5 );
	this->vecChildNVPairs.push_back( childNVP6 );
	this->vecChildNVPairs.push_back( childNVP8 );
	return;
}

void cNameValuePair::AddChild( const cNameValuePair &childNVP )
{
	this->vecChildNVPairs.push_back( childNVP );
	return;
}

bool cNameValuePair::bHadChildren(void)
{
	return ( ! this->vecChildNVPairs.empty() );
}


bool cNameValuePair::ScanChildrenForName( std::string nameToFind, cNameValuePair &childNVP, bool bScanRecursively /*= false*/ )
{
	for ( std::vector<cNameValuePair>::iterator itChildNVPair = this->vecChildNVPairs.begin();
		  itChildNVPair != this->vecChildNVPairs.end(); itChildNVPair++ )
	{
		if ( itChildNVPair->name == nameToFind )
		{
			childNVP = (*itChildNVPair);
			return true;
		}
		// Scan children's children?
		if ( bScanRecursively )
		{
			if ( itChildNVPair->ScanChildrenForName( nameToFind, childNVP, bScanRecursively ) )
			{
				return true;
			}
		}// if ( bScanRecursively )
	}// for ( std::vector<CNameValuePair>::iterator itChildNVPair 

	// Sorry it didn't work out...
	return false;
}
