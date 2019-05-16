//
// Written by Michael Feeney, Fanshawe College, (c) 2010-2014, mfeeney(at)fanshawec.ca
// For the 'Game Development - Advanced Programming' curriculum:  www.fanshawec.ca/en/GDP1
// It may be distributed under the terms of the MIT License: http://opensource.org/licenses/MIT
// Use this code at your own risk. It is indented only as a learning aid.
//
#include "ShaderTypes.h"
#include <sstream>
#include <algorithm>	// for sort()

CShaderAttributeDescription::CShaderAttributeDescription() 
{
	this->Clear();
}

void CShaderAttributeDescription::Clear( void )
{
	this->programName = "";
	this->programID = 0;
	this->name = "";
	this->index = 0;
	this->size = 0;
	this->type = GL_ID_UNKNOWN;
	this->typeAsString = "";
	return;
}

CShaderUniformDescription::CShaderUniformDescription() 
{
	this->Clear();
	return;
}

void CShaderUniformDescription::Clear( void )
{
	this->programName = "";
	this->programID = 0;
	this->name = "";
	this->index = 0;
	this->location = 0;
	this->size = 0;
	this->type = GL_ID_UNKNOWN;
	this->typeAsString = "";
	// Added
	this->namedBlockIndex = -1;	// -1 if in default block
	this->blockOffsetInBytes = -1;
	this->blockOffsetInFloats = -1;
	this->bIsInDefaultBlock = true;
	this->nameLength = 0;
	this->arrayStride = -1;
	this->bIsAnArray = false;
	this->matrixStride = -1;
	this->bIsRowMajor = false;
	this->bIsMatrix = false;
	return;
}

CShaderUniformBlockDescription::CShaderUniformBlockDescription()
{
	this->Clear();
	return;
}

void CShaderUniformBlockDescription::Clear(void)
{
	this->programID = 0;
	this->nameLength = 0;			// GL_UNIFORM_BLOCK_NAME_LENGTH?
	this->bindingPoint = 0;			// GL_UNIFORM_BLOCK_BINDING
	this->dataSize = 0;				// GL_UNIFORM_BLOCK_DATA_SIZE

	this->bReferencedFromVertexShader = false;
	this->bReferencedFromTessControlShader = false;
	this->bReferencedFromTessEvaluationShader = false;
	this->bReferencedFromGeometryShader = false;
	this->bReferencedFromFragmentShader = false;
	this->bReferencedFromComputeShader = false;
	return;
}

CShaderDescription::CShaderDescription()
{
	this->ID = 0;
	this->name = "";
	this->type = GLSHADERTYPES::UNKNOWN;
	this->source = "";
	this->filename = "";
	this->bIsOK = false;
	return;
};

CShaderDescription::CShaderDescription( std::string name, GLSHADERTYPES::enumShaderType shaderType, 
									    std::string source, std::string fileName )
{
	this->ID = 0;
	this->name = name;
	this->type = shaderType;
	this->source = source;
	this->filename = fileName;
	this->bIsOK = false;
	return;
}


CShaderDescription::~CShaderDescription() { }

CShaderProgramDescription::CShaderProgramDescription()
{
	this->ID = 0;
	this->name = "";
	this->bIsOK = false;
	return;
}

CShaderProgramDescription::CShaderProgramDescription( std::string shaderProgName, 
													  CShaderDescription vShader, 
													  CShaderDescription fShader )
{
	this->ID = 0;
	this->name = shaderProgName;
	this->bIsOK = false;
	this->vShader = vShader;
	this->fShader = fShader;
	return;
}

CShaderProgramDescription::CShaderProgramDescription( std::string shaderProgName, 
													  CShaderDescription vShader, 
													  CShaderDescription fShader, 
													  CShaderDescription gShader )
{
	this->ID = 0;
	this->name = shaderProgName;
	this->bIsOK = false;
	this->vShader = vShader;
	this->fShader = fShader;
	this->gShader = gShader;
	return;
}


CShaderProgramDescription::~CShaderProgramDescription() { }



//GLuint CShaderProgramDescription::getUniformVarIDFromName( std::string variableName )
//{
//	std::map< std::string /*uniformName*/, unsigned int /*index*/ >::iterator 
//		itUniVar = this->mapUniformVarName_to_Index.find(variableName);
//	if ( itUniVar == this->mapUniformVarName_to_Index.end() )
//	{	// Didn't find it
//		return 0;
//	}
//	GLuint ID_for_Debug = itUniVar->second;
//	return itUniVar->second;
//}

GLuint CShaderProgramDescription::getUniformLocationFromName( std::string variableName )
{
	std::map< std::string /*uniformName*/, GLuint /*location*/ >::iterator 
		itUniVar = this->mapUniformVarName_to_Location.find(variableName);
	if ( itUniVar == this->mapUniformVarName_to_Location.end() )
	{	// Didn't find it
		return 0;
	}
	GLuint ID_for_Debug = itUniVar->second;
	return itUniVar->second;
}

GLuint CShaderProgramDescription::getUniformIndexFromName( std::string variableName )
{
	std::map< std::string /*uniformName*/, GLuint /*index*/ >::iterator 
		itUniVar = this->mapUniformName_to_Index.find(variableName);
	if ( itUniVar == this->mapUniformName_to_Index.end() )
	{	// Didn't find it
		return 0;
	}
	GLuint ID_for_Debug = itUniVar->second;
	return itUniVar->second;
}



bool CShaderProgramDescription::GetUniformVarDesc( GLuint index, CShaderUniformDescription &UniformVarDesc )
{
	if ( index >= static_cast<GLuint>( this->vecUniformVariables.size() ) )
	{	// Invalid index
		return false;
	}
	UniformVarDesc = this->vecUniformVariables[index];
	return true;
}

bool CShaderProgramDescription::GetUniformVarDesc( const std::string &name, CShaderUniformDescription &UniformVarDesc )
{
	GLuint index = this->getUniformLocationFromName( name );
	if ( index == 0 )
	{	// Doesn't exist
		return false;
	}
	UniformVarDesc = this->vecUniformVariables[index];
	return true;
}

bool CShaderProgramDescription::GetNamedBlock( GLuint index, CShaderUniformBlockDescription &NamedBlockDesc )
{
	if ( index >= static_cast<GLuint>( this->vecNamedUniformBlocks.size() ) )
	{	// Invlid named block index
		return false;
	}
	NamedBlockDesc = this->vecNamedUniformBlocks[index];
	return true;
}

bool CShaderProgramDescription::GetNamedBlock( const std::string &name, CShaderUniformBlockDescription &NamedBlockDesc )
{
	std::map< std::string /*block name*/, GLint /*blockID*/ >::iterator itNB = this->mapNamedUniformBlockName_to_Index.find( name );
	if ( itNB == this->mapNamedUniformBlockName_to_Index.end() )
	{	// Doesn't exist
		return false;
	}
	NamedBlockDesc = this->vecNamedUniformBlocks[itNB->second];
	return true;
}


std::string CShaderProgramDescription::getErrorString( void )
{
	std::stringstream ss;
	std::vector< std::string >::iterator itErr;
	// Copy the various shader errors to the main error string 
	// Vertex shader errors...
	if ( !this->vShader.vecShaderErrors.empty() )
	{
		ss << "Vertex shader errors:" << std::endl;
		for ( itErr = this->vShader.vecShaderErrors.begin(); itErr != this->vShader.vecShaderErrors.end(); itErr++ )
		{
			ss << *itErr << std::endl;
		}
		ss << std::endl;
	}
	// Fragment shader errors...
	if ( !this->fShader.vecShaderErrors.empty() )
	{
		ss << "Fragment shader errors:" << std::endl;
		for ( itErr = this->fShader.vecShaderErrors.begin(); itErr != this->fShader.vecShaderErrors.end(); itErr++ )
		{
			ss << *itErr << std::endl;
		}
		ss << std::endl;
	}
	// Geometry shader errors...
	if ( !this->gShader.vecShaderErrors.empty() )
	{
		ss << "Geometry shader errors:" << std::endl;
		for ( itErr = this->gShader.vecShaderErrors.begin(); itErr != this->gShader.vecShaderErrors.end(); itErr++ )
		{
			ss << *itErr << std::endl;
		}
		ss << std::endl;
	}
	// And program (linker) errors:
	ss << "General program errors:" << std::endl;
	for ( itErr = this->vecErrors.begin(); itErr != this->vecErrors.end(); itErr++ )
	{
		ss << *itErr << std::endl;
	}
	return ss.str();
}


	// Predicate functions used for sorting 
/*static*/ 
bool CShaderProgramDescription::m_pfSortAttribByIndex( const CShaderAttributeDescription &a1, const CShaderAttributeDescription &a2 )
{	
	if ( a1.index < a2.index )	{ return true; }
	return false;
}

/*static*/ 
bool CShaderProgramDescription::m_pfSortUniformByIndex( const CShaderUniformDescription &u1, const CShaderUniformDescription &u2 )
{
	if ( u1.index < u2.index )	{ return true; }
	return false;
}



std::string CShaderProgramDescription::getActiveAttribsPrintableSummary(bool bSortByIndex = true)
{
	std::stringstream ss;
	if ( this->mapAttributesByName.empty() )
	{
		ss << "No attributes found in this shader program (" << this->name << ")." << std::endl;
	}
	else
	{
		ss << "Active ATTRIBUTES defined in this shader program (" << this->name << "):" << std::endl;
	}

	// Store the attributes into a vector so we can sort them by index
	std::vector< CShaderAttributeDescription > vecTempAttribs;
	for ( std::map< std::string /*attribName*/, CShaderAttributeDescription >::iterator itAttrib = this->mapAttributesByName.begin();
		  itAttrib != this->mapAttributesByName.end(); ++itAttrib )
	{
		vecTempAttribs.push_back( itAttrib->second );
	}
	
	if ( bSortByIndex )	{ std::sort( vecTempAttribs.begin(), vecTempAttribs.end(), CShaderProgramDescription::m_pfSortAttribByIndex ); }
	// 

	for ( std::vector< CShaderAttributeDescription >::iterator itAttrib = vecTempAttribs.begin();
			itAttrib != vecTempAttribs.end(); ++itAttrib )
	{
		ss << "Index [" << itAttrib->index << "], ";
		ss << "name: \"" << itAttrib->name << "\", ";
		ss << "size: " << itAttrib->size << ", ";		// NOT bytes, but number of variables: 1 = single, or GT 1 for an array
		ss << "type: " << itAttrib->typeAsString << std::endl;
	}

	return ss.str();
}

void gAddSpacesToStream( std::stringstream &ss, unsigned int numSpaces )
{
	for ( int count = 0; count != numSpaces; count++ )
	{
		ss << " ";
	}
	return;
}

std::string CShaderProgramDescription::getActiveUnifomsPrintableSummary(bool bSortByIndex = true)
{
	std::stringstream ss;
	std::map< std::string /*uniformName*/, CShaderUniformDescription > mapShaderUniforms;


	//if ( this->mapShaderUniforms.empty() ) {}
	if ( this->vecUniformVariables.empty() )
	{
		ss << "No uniforms found in this shader program (" << this->name << ")." << std::endl;
	}
	else
	{
		ss << "Active UNIFORMS defined in this shader program (" << this->name << "):" << std::endl;
	}

	//// Store the attributes into a vector so we can sort them by index
	//std::vector< CShaderUniformDescription > vecTempUniforms;
	//for ( std::map< std::string /*uniformName*/, CShaderUniformDescription >::iterator itUniform = this->mapShaderUniforms.begin();
	//	  itUniform != this->mapShaderUniforms.end(); ++itUniform )
	//{
	//	vecTempUniforms.push_back( itUniform->second );
	//}

	//if ( bSortByIndex ) { std::sort( vecTempUniforms.begin(), vecTempUniforms.end(), CShaderProgramDescription::m_pfSortUniformByIndex ); }
	//
	//// 
	//for ( std::vector< CShaderUniformDescription >::iterator itUniform = vecTempUniforms.begin();
	//		itUniform != vecTempUniforms.end(); ++itUniform )

	// To make it pretty, determine how wide the widest uniform name is:
	unsigned int prettyPrintLongestNamedBlockUniformName = 0;
	unsigned int prettyPrintLongestDefaultUniformName = 0;
	for ( std::vector< CShaderUniformDescription >::iterator itUniform = this->vecUniformVariables.begin();
			itUniform != this->vecUniformVariables.end(); ++itUniform )
	{
		// Default or named block?
		if ( itUniform->bIsInDefaultBlock )
		{	// In default block
			if ( static_cast<unsigned int>( itUniform->name.length() ) > prettyPrintLongestDefaultUniformName )
			{
				prettyPrintLongestDefaultUniformName = static_cast<unsigned int>( itUniform->name.length() );
			}
		}
		else
		{	// In a named block
			if ( static_cast<unsigned int>( itUniform->name.length() ) > prettyPrintLongestNamedBlockUniformName )
			{
				prettyPrintLongestNamedBlockUniformName = static_cast<unsigned int>( itUniform->name.length() );
			}
		}// if ( itUniform->bIsInDefaultBlock )
	}// for ( std::vector< CShaderUniformDescription >::iterator itUniform ...

	for ( std::vector< CShaderUniformDescription >::iterator itUniform = this->vecUniformVariables.begin();
			itUniform != this->vecUniformVariables.end(); ++itUniform )
	{
		// Default or named uniform block first...
		if ( itUniform->bIsInDefaultBlock )
		{
			ss << "(Default block) ";
			ss << "\"" << itUniform->name << "\", ";
			// Pad name with spaces
			gAddSpacesToStream( ss, prettyPrintLongestDefaultUniformName - (unsigned int)itUniform->name.length() );
		}
		else 
		{
			ss << "(Named block \"" << itUniform->blockName << "\"";
			ss << " index:" << itUniform->namedBlockIndex << ") ";
			ss << "\"" << itUniform->name << "\", ";
			// Pad name with spaces
			gAddSpacesToStream( ss, prettyPrintLongestNamedBlockUniformName - (unsigned int)itUniform->name.length() );
		}

		ss << "Index[" << itUniform->index << "], ";
		ss << "Location:" << itUniform->location << ", ";
		ss << "offset:" << itUniform->blockOffsetInBytes << " bytes, ";
		if ( itUniform->blockOffsetInBytes > 0 )
		{
			ss << "(" << itUniform->blockOffsetInFloats << " floats), ";
		}

		ss << itUniform->typeAsString << ", "; 

		if ( itUniform->bIsAnArray )	{ ss << "IS ARRAY: " << itUniform->size << " items, "; }
		else							{ ss << "size: " << itUniform->size << ", "; }

		if ( itUniform->bIsMatrix )
		{
			if ( !itUniform->bIsRowMajor )	{	ss << "ROW MAJOR matrix ";	}
			else							{	ss << "COLOMN MAJOR matrix (default) "; }
		}

		// In default block or named block?
		if ( ! itUniform->bIsInDefaultBlock )	
		{ 	// Named
			// Print array stride (if an array)
			if ( itUniform->bIsAnArray ) 
			{
				ss << "array stride: " << itUniform->arrayStride;
			}
			
			// Print matrix stride (if a matrix)
			if ( itUniform->bIsMatrix )
			{
				ss << "matrix stride: " << itUniform->matrixStride;
			}
		}// if ( itUniform->bIsInDefaultBlock )	...

		// New line
		ss << std::endl;

	}// for ( std::vector< CShaderUniformDescription >::iterator itUniform...
	
	return ss.str();
}

std::string CShaderProgramDescription::getActiveNamedUniformBlockPrintableSummary(bool bSortByIndex)
{
	std::stringstream ss;
	for ( std::vector< CShaderUniformBlockDescription >::iterator itNUB = this->vecNamedUniformBlocks.begin();
		  itNUB != this->vecNamedUniformBlocks.end(); ++itNUB )
	{
		// Any unforms in there (i.e. is active?)
		if ( !itNUB->vecUniformVarIndices.empty() )
		{
			ss << "Uniform block name: " << itNUB->name << ", ";
			ss << "data size: " << itNUB->dataSize << ", ";
			ss << "binding point: " << itNUB->bindingPoint << std::endl;
			for ( std::vector< GLint >::iterator itVars = itNUB->vecUniformVarIndices.begin(); 
				  itVars != itNUB->vecUniformVarIndices.end(); ++itVars )
			{
				CShaderUniformDescription& currentUniform = this->vecUniformVariables[*itVars];

				ss << "\t\"" << currentUniform.name << "\", ";
				ss << "index[" << currentUniform.index << "], ";
				ss << "location: " << currentUniform.location << ", ";
				ss << "type = " << currentUniform.typeAsString << ", ";
				ss << "blockOffset:" << currentUniform.blockOffsetInBytes << " bytes ("
 					                 << currentUniform.blockOffsetInFloats << " floats), ";
				
				if ( currentUniform.bIsMatrix )
				{
					if ( currentUniform.bIsRowMajor )	{	ss << "ROW MAJOR ";	}
					else                                {	ss << "COLUMN MAJOR (default) "; }
						
					ss << "Mat stride: " << currentUniform.matrixStride << " bytes";
				}

				if ( currentUniform.bIsAnArray )
				{
					ss << "(array) size: " << currentUniform.size << std::endl;
				}
				else
				{
					ss << " size: " << currentUniform.size << std::endl;
				}

			}// for ( std::vector< GLint >::iterator itVars...
		}//if ( !itNUB->vecUniformVarIndices.empty() )
	}// for ( std::vector< CShaderUniformBlockDescription >::iterator itNUB...

	return ss.str();
}

