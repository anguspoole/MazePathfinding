#ifndef _cNameValuePair_HG_
#define _cNameValuePair_HG_

#include <string>
#include <vector>



class cNameValuePair
{
public:
	typedef struct fvec4
	{
		fvec4() : x(0.0f), y(0.0f), z(0.0), w(1.0f) {};
		float x, y, z, w;
	} fvec4;	
	cNameValuePair();
	cNameValuePair( const std::string &name );
	cNameValuePair( const std::string &name, fvec4 fVec4Value );
	cNameValuePair( const std::string &name, float fValue );
	cNameValuePair( const std::string &name, int iValue );
	cNameValuePair( const std::string &name, std::string sValue );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair childNVP3 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair &childNVP3, const cNameValuePair &childNVP4 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
					const cNameValuePair &childNVP6 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
					const cNameValuePair &childNVP6, const cNameValuePair &childNVP7 );
	cNameValuePair( const std::string &name, const cNameValuePair &childNVP0, const cNameValuePair &childNVP1, const cNameValuePair &childNVP2, 
		            const cNameValuePair &childNVP3, const cNameValuePair &childNVP4, const cNameValuePair &childNVP5, 
					const cNameValuePair &childNVP6, const cNameValuePair &childNVP7, const cNameValuePair &childNVP8 );

	void AddChild( const cNameValuePair &childNVP );
	bool bHadChildren(void);
	std::string name;
	// ** The values **
	fvec4 fvec4Value;		// Can also be used for one float
	int iValue;
	std::string sValue;
	// ****************
	void Reset(void);
	std::vector<cNameValuePair> vecChildNVPairs;
	// Helper function to scan through the children, looking for certain commands
	// Will only scan one level deep unless you tell it to (but is that a Good Idea?)
	// Will return after finding the first one. If you want to "beef this up", you could 
	//	emulate the STL find() and find_next() type methods which can take a starting iterator, which you can 
	//  pass the iterator in as a starting point
	bool ScanChildrenForName( std::string nameToFind, cNameValuePair &childNVP, bool bScanRecursively = false );
};




#endif 











//typedef struct fvec4
//{
//	fvec4() : x(0.0f), y(0.0f), z(0.0), w(1.0f) {};
//	float x, y, z, w;
//} fvec4;
//
//union TheDataTypes
//{
//	// VS work-around from: http://stackoverflow.com/questions/21968635/invalid-union-member
//	struct {
//		fvec4 fVec4;
//	};
//	float fValue;
//	bool bValue;
//	int iValue;
//	unsigned int uiValue;
//};
//
//class CNameValuePair
//{
//public:
//	enum enumTypeInUse
//	{
//		IS_A_STRING,
//		IS_A_VEC4,
//		IS_A_FLOAT,
//		IS_AN_INT,
//		IS_AN_UINT,
//		UNKNOWN_OR_NOT_SET
//	};