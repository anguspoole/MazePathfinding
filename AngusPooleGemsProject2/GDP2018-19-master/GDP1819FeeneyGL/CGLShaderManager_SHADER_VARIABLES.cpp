// Written by Michael Feeney, Fanshawe College, 2010
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.
//
#include "CGLShaderManager.h"
#include <sstream>
#include <fstream>
//#include "OpenGLExt.h"
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr






bool CGLShaderManager::IsUniformInNamedBlock( std::string shaderName, std::string uniformName )
{
	if ( ! this->DoesShaderExist( shaderName ) ) { return false; }
	
	GLuint shaderID = this->m_mapShaderName_to_ID[shaderName];

	return this->IsUniformInNamedBlock( shaderID, uniformName );
}

bool CGLShaderManager::IsUniformInNamedBlock( GLuint shaderID, std::string uniformName )
{
	if ( ! this->DoesShaderExist( shaderID ) ) { return false; }

	CShaderProgramDescription& currentShader = this->m_mapShaderProgram[shaderID];

	std::map< std::string /*uniformName*/, unsigned int /*index*/ >::iterator itUniformID = currentShader.mapUniformVarName_to_Location.find(uniformName);
	if ( itUniformID == currentShader.mapUniformVarName_to_Location.end() )
	{	// Uniform doesn't exist
		return false;
	}

	return !(currentShader.vecUniformVariables[ itUniformID->second ].bIsInDefaultBlock);
}


// These all encapsulate both the glGetUniformLocation() and the glUniformXX() functions
//	as well as the glGetAttribLocation() and glVertexAttribXX() functions

 //      _ _   _      _  __               ___  __
 // __ _| | | | |_ _ (_)/ _|___ _ _ _ __ / \ \/ /
 /// _` | | |_| | ' \| |  _/ _ \ '_| '  \| |>  < 
 //\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|_/_/\_\
 //|___/                                         

// glUniform1f...
bool CGLShaderManager::SetUniformVar1f( const std::string &shaderName, const std::string &varName, GLfloat value )
{	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniformVar1f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar1f( shaderID, varName, value);
}

bool CGLShaderManager::SetUniformVar1f( GLuint shaderID, const std::string &varName, GLfloat value )
{	// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniformVar1f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniformVar1f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform1f( varIndex, value );	
	return true;
}

bool CGLShaderManager::SetUniformVar1fNamedBlock( const std::string &shaderName, const std::string &varName, GLfloat value )
{
	GLuint shaderID = GetShaderIDFromName( shaderName );
	if ( shaderID == 0 )
	{	// Shader doesn't exist
		return false;
	}
	return this->SetUniformVar1fNamedBlock( shaderID, varName, value );
}

// Returns false if it's not there or not correct type
// Note: Ignores array types, in that it doesn't check array bounds (caller responsible for this)
// Needs: ShaderID, variableName, type
// Returns (by ref): Uniform Variable 
bool CGLShaderManager::m_GetUniformInNamedBlockInfo( GLuint shaderID, const std::string &varName, 
													 CShaderUniformDescription& currentUniform,
													 GLint &bufferBindingPoint )
{
	CShaderProgramDescription shaderProgDescription;
	shaderProgDescription.ID = shaderID;
	// Get information for this shader...
	if ( ! this->GetShaderProgramInfo( shaderProgDescription ) )	{ return false; }	// Shader doesn't exist

	// Get the information for this particular variable...
	//unsigned int varIndex = shaderProgDescription.getUniformLocationFromName( varName );
	// (Since it's in a NAMED uniform block, we get the index...?)
	unsigned int varIndex = shaderProgDescription.getUniformIndexFromName( varName );

	if ( varIndex == 0 ) { return false; }	// Invalid variable index

	currentUniform = shaderProgDescription.vecUniformVariables[varIndex];
	// In named block?
	if ( ! currentUniform.bIsInDefaultBlock ) { return false; }	// nope, it's in the default block
	
	bufferBindingPoint = shaderProgDescription.vecNamedUniformBlocks[currentUniform.namedBlockIndex].bindingPoint;
	if ( bufferBindingPoint < 0 )	{ return false; }	// Binding point is invalid

	// Good to go
	return true;
}

bool CGLShaderManager::SetUniformVar1fNamedBlock( GLuint shaderID, const std::string &varName, GLfloat value )
{
	CShaderUniformDescription currentUniform;
	GLint bufferBindingPoint = 0;
	if ( ! this->m_GetUniformInNamedBlockInfo( shaderID, varName, currentUniform, bufferBindingPoint ) )
	{
		return false;
	}
	// Right type?
	if ( currentUniform.type != GL_FLOAT )	{ return false;	} // Wrong type

	*(this->m_vecUniformBufferBindings[bufferBindingPoint].pBufferData + currentUniform.blockOffsetInFloats) = value;

	return true;
}

bool CGLShaderManager::SetUniformVer1fNamedBlockArray( GLuint shaderID, const std::string &varName, 
													   GLfloat* pValues, int arraySize )
{
	CShaderUniformDescription currentUniform;
	GLint bufferBindingPoint = 0;
	if ( ! this->m_GetUniformInNamedBlockInfo( shaderID, varName, currentUniform, bufferBindingPoint ) )
	{
		return false;
	}
	// Right type? 
	if ( currentUniform.type != GL_FLOAT )	{ return false; }	// Wrong type

	// An array?
	if ( !currentUniform.bIsAnArray )	{ return false; } // Variable isn't an array

	// Passing more numbers than are in array?
	if ( arraySize >= currentUniform.size )	{ return false; }	// Too big for array


	for ( int index = 0; index != arraySize; index++ )
	{
		*(this->m_vecUniformBufferBindings[bufferBindingPoint].pBufferData + currentUniform.blockOffsetInFloats) = pValues[index];
		// Note: this is a local variable, so we can change it as we're throwing it away at end of method
		currentUniform.blockOffsetInFloats += currentUniform.arrayStride;
	}

	return true;
}


// glUniform1i...
bool CGLShaderManager::SetUniformVar1i( std::string shaderName, std::string varName, GLint value )
{	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniformVar1i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar1i( shaderID, varName, value);
}
bool CGLShaderManager::SetUniformVar1i( GLuint shaderID, std::string varName, GLint value )
{	// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniformVar1f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniformVar1i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform1i( varIndex, value );	
	return true;
}

// glUniform1ui... (OpenGL 3.0)
bool CGLShaderManager::SetUniformVar1ui( std::string shaderName, std::string varName, GLint value )
{	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniformVar1ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar1ui( shaderID, varName, value);
}

bool CGLShaderManager::SetUniformVar1ui( GLuint shaderID, std::string varName, GLint value )
{	// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniformVar1ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniformVar1ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform1ui( varIndex, value );	
	return true;
}

//      _ _   _      _  __               _____  __
// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |_  ) \/ /
/// _` | | |_| | ' \| |  _/ _ \ '_| '  \ / / >  < 
//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_/___/_/\_\
//|___/                                           
bool CGLShaderManager::SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2 )
{	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2f( shaderID, varName, v1, v2 );
}

bool CGLShaderManager::SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v[2] )
{	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2f( shaderID, varName, v[0], v[1] );
}


bool CGLShaderManager::SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2f( varIndex, v1, v2 );	
	return true;
}

bool CGLShaderManager::SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v[2] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2f( varIndex, v[0], v[1] );	
	return true;
}

bool CGLShaderManager::SetUniformVar2f( GLuint shaderID, std::string varName, glm::vec2 v2 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2f( varIndex, v2.x, v2.y );	
	return true;
}

bool CGLShaderManager::SetUniformVar2i( std::string shaderName, std::string varName, GLint v1, GLint v2 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2i( shaderID, varName, v1, v2 );
}

bool CGLShaderManager::SetUniformVar2i( std::string shaderName, std::string varName, GLint v[2] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2i( shaderID, varName, v[0], v[1] );
}

bool CGLShaderManager::SetUniformVar2i( GLuint shaderID, std::string varName, GLint v1, GLint v2 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2i( varIndex, v1, v2 );	
	return true;
}

bool CGLShaderManager::SetUniformVar2i( GLuint shaderID, std::string varName, GLint v[2] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2i( varIndex, v[0], v[1] );	
	return true;
}


bool CGLShaderManager::SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2ui( shaderID, varName, v1, v2 );
}

bool CGLShaderManager::SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v[2] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar2ui( shaderID, varName, v[0], v[1] );
}

bool CGLShaderManager::SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2ui( varIndex, v1, v2 );	
	return true;
}

bool CGLShaderManager::SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v[2])
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform2ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform2ui( varIndex, v[0], v[1] );	
	return true;
}


// ************************************
//      _ _   _      _  __               ______  __
// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |__ /\ \/ /
/// _` | | |_| | ' \| |  _/ _ \ '_| '  \ |_ \ >  < 
//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|___//_/\_\
//|___/                                            

bool CGLShaderManager::SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3f( shaderID, varName, v1, v2, v3 );
}

bool CGLShaderManager::SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v[3] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3f( shaderID, varName, v[0], v[1], v[2] );
}
bool CGLShaderManager::SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3f( varIndex, v1, v2, v3 );	
	return true;
}

bool CGLShaderManager::SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat v[3] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3f( varIndex, v[0], v[1], v[2] );	
	return true;
}

bool CGLShaderManager::SetUniformVar3f( GLuint shaderID, std::string varName, glm::vec3 v3 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3f( varIndex, v3.x, v3.y, v3.z );	
	return true;
}

bool CGLShaderManager::SetUniformVar3i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3i( shaderID, varName, v1, v2, v3 );
}
bool CGLShaderManager::SetUniformVar3i( std::string shaderName, std::string varName, GLint v[3] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3i( shaderID, varName, v[0], v[1], v[2] );
}

bool CGLShaderManager::SetUniformVar3i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3i( varIndex, v1, v2, v3 );	
	return true;
}

bool CGLShaderManager::SetUniformVar3i( GLuint shaderID, std::string varName, GLint v[3] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3i( varIndex, v[0], v[1], v[2] );	
	return true;
}


bool CGLShaderManager::SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3ui( shaderID, varName, v1, v2, v3 );
}

bool CGLShaderManager::SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v[3] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar3ui( shaderID, varName, v[0], v[1], v[2] );
}

bool CGLShaderManager::SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3ui( varIndex, v1, v2, v3 );	
	return true;
}

bool CGLShaderManager::SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v[3] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform3ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform3ui( varIndex, v[0], v[1], v[2] );	
	return true;
}





//      _ _   _      _  __              _ ___  __
// __ _| | | | |_ _ (_)/ _|___ _ _ _ __| | \ \/ /
/// _` | | |_| | ' \| |  _/ _ \ '_| '  \_  _>  < 
//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_||_/_/\_\
//|___/                                          
bool CGLShaderManager::SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4f( shaderID, varName, v1, v2, v3, v4 );
}

bool CGLShaderManager::SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v[4] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4f( shaderID, varName, v[0], v[1], v[2], v[3] );
}

bool CGLShaderManager::SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4f( varIndex, v1, v2, v3, v4 );	
	return true;
}

bool CGLShaderManager::SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v[4] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4f( varIndex, v[0], v[1], v[2], v[3] );	
	return true;
}

bool CGLShaderManager::SetUniformVar4f( GLuint shaderID, std::string varName, glm::vec4 v4 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4f: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4f( varIndex, v4.x, v4.y, v4.z, v4.w );	
	return true;
}



bool CGLShaderManager::SetUniformVar4i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4i( shaderID, varName, v1, v2, v3, v4 );
}

bool CGLShaderManager::SetUniformVar4i( std::string shaderName, std::string varName, GLint v[4] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4i( shaderID, varName, v[0], v[1], v[2], v[3] );
}


bool CGLShaderManager::SetUniformVar4i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4i( varIndex, v1, v2, v3, v4 );	
	return true;
}

bool CGLShaderManager::SetUniformVar4i( GLuint shaderID, std::string varName, GLint v[4] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4i: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4i( varIndex, v[0], v[1], v[2], v[3] );	
	return true;
}


bool CGLShaderManager::SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4ui( shaderID, varName, v1, v2, v3, v4 );
}

bool CGLShaderManager::SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v[4] )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformVar4ui( shaderID, varName, v[0], v[1], v[2], v[3]);
}

bool CGLShaderManager::SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4ui( varIndex, v1, v2, v3, v4 );	
	return true;
}

bool CGLShaderManager::SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v[4] )
{
// Valid ID?
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...
	glUniform4ui( varIndex, v[0], v[1], v[2], v[3] );	
	return true;
}

bool CGLShaderManager::SetUniformMatrix4fv( std::string shaderName, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	// Valid name?
	GLint shaderID = this->GetShaderIDFromName ( shaderName );
	if ( shaderID < 0 ) 
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader '" << shaderName << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	return this->SetUniformMatrix4fv( shaderID, varName, count, transpose, value );
}

bool CGLShaderManager::SetUniformMatrix4fv( GLuint shaderID, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...

	glUniformMatrix4fv( varIndex, count, transpose, value );	
	return true;	
}

bool CGLShaderManager::SetUniformMatrixGLM_Mat4x4( GLuint shaderID, std::string varName, GLboolean transpose, const glm::mat4x4 &matValue )
{
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// Valid ID...
	GLint varIndex = glGetUniformLocation( shaderID, varName.c_str() );
	if ( varIndex < 0 )
	{
		std::stringstream ss; ss << "SetUniform4ui: Can't find uniform variable '" 
			<< varName << "' in shader ID '" << shaderID << "'";
		this->m_LastError = ss.str();
		return false;
	}
	// It's all good, so set it, baby...

	glUniformMatrix4fv( varIndex, 1, (transpose ? GL_TRUE : GL_FALSE), glm::value_ptr(matValue) );

	return true;	
}

