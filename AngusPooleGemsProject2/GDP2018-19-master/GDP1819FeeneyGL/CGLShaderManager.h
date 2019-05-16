// Written by Michael Feeney, Fanshawe College, 2010-2019
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.
//
#ifndef _CGLShaderManager_HG_
#define _CGLShaderManager_HG_

#include "IGLShaderManager.h"
// ***************************************************************
// ** Objects that use this should include the IGLShaderManager **
// ** Objects that need to create this (should only be one),    **
// **   should include the CGLShaderManager.                    **
// ***************************************************************

#include <map>

class CGLShaderManager : public IGLShaderManager
{
public:
	CGLShaderManager();
	virtual ~CGLShaderManager();
	//
	virtual void SetBaseFilePath( std::string baseFilePath );
	virtual std::string GetBaseFilePath(void);

	virtual bool CreateShaderProgramFromSource( CShaderProgramDescription &shaderProgDescription );
	virtual bool CreateShaderProgramFromFile( CShaderProgramDescription &shaderProgDescription );

	virtual bool CreateShaderProgramsFromSources( std::vector<CShaderProgramDescription> &shaderProgDescriptions );
	virtual bool CreateShaderProgramsFromFiles( std::vector<CShaderProgramDescription> &shaderProgDescriptions );

	//virtual bool UseShaderProgram( GLuint shaderID );
	//virtual bool UseShaderProgram( std::string shaderName );
	// Updated: Normally, if you pass the same ID or name, the shader isn't changed
	virtual bool UseShaderProgram( GLuint shaderID, bool bForceShaderProgramSet );
	virtual bool UseShaderProgram( std::string shaderName, bool bForceShaderProgramSet );

	// Destroys everything (called when program exits or you are deleting all shaders)
	virtual bool ShutDown(void); 

	// Returns zero (0) and blank string if no valid shader exists
	virtual GLuint GetCurrentShaderID(void);
	virtual GLuint GetLastShaderID(void);
	virtual std::string GetCurrentShaderName(void);
	virtual std::string GetLastShaderName(void);

	virtual bool GetShaderProgramInfo( CShaderProgramDescription &shaderProgDescription );

	// Uses glGetActiveAttrib and glGetActiveUniform to determine the active variables in use in the shader
	// BIG FAT NOTE: This is only the "active" ones - many times, they are profiled out by the compiler
	virtual bool LoadActiveShaderVariables( GLuint shaderID );
	virtual bool LoadActiveShaderVariables( std::string shaderName );

	// Returns zero (0) if not found
	virtual GLint GetShaderIDFromName( std::string shaderName );

	virtual bool DoesShaderExist( GLuint shaderProgramID );
	virtual bool DoesShaderExist( std::string shaderName );

	virtual bool IsShaderOK( GLuint shaderProgramID );
	virtual bool IsShaderOK( std::string shaderName );

	//virtual bool GetAttributeInfo( CShaderAttributeDescription &attributeDescription );

	virtual std::string DecodeTypeNameFromGLenum( GLenum type );

	virtual std::string GetLastError(void);
	virtual bool bIsErrorTextPresent(void);

	/*************************************************************
		After this is the are endless repeated variations of the 
		varying and uniform variable accessors...
	*************************************************************/

	// These are all tied to the vector of binding points
	// Each binding point is connected to a buffer
	// Here, there is a vector of binding points, which have 

	// This represents a buffer that is tied to one or more uniform blocks
	class CUniformBlockBuffer
	{
	public:
		CUniformBlockBuffer() : bufferID(0), pBufferData(0), bindingPoint(0), 
			                    bufferSizeBytes(0), bufferSizeFloats(0), bInUse(false) {};
		GLuint bufferID;
		GLuint bindingPoint;	// From 0 to GL_MAX_UNIFORM_BUFFER_BINDINGS
		// GLint uniformBlockID;		// Are the IDs of each shader uniform block unique or the same?
		std::string uniformBlockName;
		float* pBufferData;
		unsigned int bufferSizeBytes;
		unsigned int bufferSizeFloats;
		bool bInUse; // = false;
	};
	std::vector< CUniformBlockBuffer > m_vecUniformBufferBindings;
	std::map< std::string /*name*/, GLuint /*bindingPoint*/ > m_mapBufferNameToBindingPoint;

	// Creates at buffer for the named uniform block. 
	// Note: if the uniform block names are the same, then the buffers are shared (so not created if already present)
	// (Why would you NOT want to share the named uniform block with the same name?)
	// This creates or uses a buffer for a named uniform block within a shader.
	// Note: If no block exists, one will be created an assigned.
	//       If a block already exists, it will assigned to the shader.
	//       The first block created is the 'referece' block; any subsequent
	//        named blocks must match this one (though the checking is not super detailed). 
	virtual bool InitBufferFromNamedUniformBlock( GLuint shaderProgramID, const std::string &uniformBlockName );
	virtual bool InitBufferFromNamedUniformBlock( const std::string &shaderProgramName, const std::string &uniformBlockName );
	// Creates all the buffers for all the named uniform blocks for the particular shader
	virtual bool InitBuffersFromUniformBlocks( GLuint shaderProgramID );
	virtual bool InitBuffersFromUniformBlocks( const std::string &shaderProgramName );
	// Should this be in the shader object?
	// This copies the client side buffer to the shader binding point.
	// Note: This does no checking to see if the information is valid, just that the buffer exists and is in use
	virtual bool UpdateNamedBlockFromBuffer( GLuint shaderProgramID, const std::string &uniformBlockName );
	virtual bool UpdateNamedBlockFromBuffer( const std::string &shaderProgramName, const std::string &uniformBlockName );

	// This generates a custom CNamedUnformBlockBufferVariableAdapter, named after the uniform block and shader, 
	//	which inherits from the base CNamedUnformBlockBufferVariableAdapter class, but has specific methods 
	//  and sub classes for the various uniforms contained inside.
	//virtual bool GenerateCustomNamedUnformBlockBufferVariableAdapterClass( const std::string &shaderProgramName, const std::string &uniformBlockName, std::string &source );

	GLuint GetMaxUniformBindings(void);			// Calls	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, ... );
	GLuint m_maxUniformBindings;


	bool IsUniformInNamedBlock( std::string shaderName, std::string uniformName );
	bool IsUniformInNamedBlock( GLuint shaderID, std::string uniformName );

	// In progress
	bool m_GetUniformInNamedBlockInfo( GLuint shaderID, const std::string &varName, 
		                               CShaderUniformDescription& uniformInfo, GLint &bufferBindingPoint );
	
	 //      _ _   _      _  __               ___  __
	 // __ _| | | | |_ _ (_)/ _|___ _ _ _ __ / \ \/ /
	 /// _` | | |_| | ' \| |  _/ _ \ '_| '  \| |>  < 
	 //\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|_/_/\_\
	 //|___/                                         
	// glUniform1f...
	virtual bool SetUniformVar1f( const std::string &shaderName, const std::string &varName, GLfloat value );			
	virtual bool SetUniformVar1f( GLuint shaderID, const std::string &varName, GLfloat value );	
	virtual bool SetUniformVar1fNamedBlock( const std::string &shaderName, const std::string &varName, GLfloat value );
	virtual bool SetUniformVar1fNamedBlock( GLuint shaderID, const std::string &varName, GLfloat value );
	// Should this call be the same as above??
	//virtual bool SetUniformVer1fNamedBlockArray( const std::string &shaderName, const std::string &varName, GLfloat* pValues, int arraySize );
	virtual bool SetUniformVer1fNamedBlockArray( GLuint shaderID, const std::string &varName, GLfloat* pValues, int arraySize );
	//virtual bool SetUniformVer1fNamedBlockArray( const std::string &shaderName, const std::string &varName, GLfloat value, int index );
	
	// glUniform1i...
	virtual bool SetUniformVar1i( std::string shaderName, std::string varName, GLint value );
	virtual bool SetUniformVar1i( GLuint shaderID, std::string varName, GLint value );
	// glUniform1ui... (OpenGL 3.0)
	virtual bool SetUniformVar1ui( std::string shaderName, std::string varName, GLint value );	// OpenGL 3.0
	virtual bool SetUniformVar1ui( GLuint shaderID, std::string varName, GLint value );			// OpenGL 3.0

	//      _ _   _      _  __               _____  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |_  ) \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \ / / >  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_/___/_/\_\
	//|___/                                           
	// glUniform2f
	virtual bool SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2 );
	virtual bool SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v[2] );
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2 );
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v[2] );
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, glm::vec2 v2 );
	// glUniform2i
	virtual bool SetUniformVar2i( std::string shaderName, std::string varName, GLint v1, GLint v2 );
	virtual bool SetUniformVar2i( std::string shaderName, std::string varName, GLint v[2] );
	virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, GLint v1, GLint v2 );
	virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, GLint v[2] );
	//virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, glm::ivec2 v2 );

	// glUniform2i
	virtual bool SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2 );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v[2] );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2 );		// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v[2] );		// OpenGL 3.0 - shader version 130

	//      _ _   _      _  __               ______  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |__ /\ \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \ |_ \ >  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|___//_/\_\
	//|___/                                            
	// glUniform3f
	virtual bool SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 );
	virtual bool SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v[3] );
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 );
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat v[3] );
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, glm::vec3 v3 );

	// glUniform3i
	virtual bool SetUniformVar3i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3 );
	virtual bool SetUniformVar3i( std::string shaderName, std::string varName, GLint v[3] );
	virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3 );
	virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, GLint v[3] );
	//virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, glm::ivec3 v3 );

	// glUniform3ui
	virtual bool SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3 );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v[3] );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3 );			// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v[3] );			// OpenGL 3.0 - shader version 130


	//      _ _   _      _  __              _ ___  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __| | \ \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \_  _>  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_||_/_/\_\
	//|___/                                          
	// glUniform4f
	virtual bool SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 );
	virtual bool SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v[4] );
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 );
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v[4] );
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, glm::vec4 v4 );

	// glUniform4i
	virtual bool SetUniformVar4i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 );
	virtual bool SetUniformVar4i( std::string shaderName, std::string varName, GLint v[4] );
	virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 );
	virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, GLint v[4] );
	//virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, glm::ivec4 v4 );

	// glUniform4ui
	virtual bool SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v[4] );	// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 );			// OpenGL 3.0 - shader version 130
	virtual bool SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v[4] );			// OpenGL 3.0 - shader version 130

	virtual bool SetUniformMatrix4fv( std::string shaderName, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value );
	virtual bool SetUniformMatrix4fv( GLuint shaderID, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value );
	virtual bool SetUniformMatrixGLM_Mat4x4( GLuint shaderID, std::string varName, GLboolean transpose, const glm::mat4x4 &value );

	//PFNGLUNIFORM2FPROC				glUniform2f = 0;
	//PFNGLUNIFORM2IPROC				glUniform2i = 0;
	//PFNGLUNIFORM2UIPROC				glUniform2ui = 0;

private:
	std::map< GLuint, CShaderProgramDescription >	m_mapShaderProgram;			// Holds the shader information
	std::map< std::string, GLuint>					m_mapShaderName_to_ID;		// Used by UseShaderProgram (by name)

	bool m_LoadShaderFromFile( std::string shaderFileName, std::string &shaderSource );

	std::string m_LastError;

	GLuint m_currentShaderID;
	GLuint m_lastShaderID;

	std::string m_baseFilePath;

	// returns false if no error
	bool m_wasThereACompileError( unsigned int shaderID, std::string &errorText );
	bool m_wasThereALinkError( unsigned int progID, std::string &errorText );


	bool m_compileShaderFromSource_OLD_DONT_USE( CShaderDescription &shader, std::string &error );



	static const unsigned int MAX_SHADER_CHAR_BUFFER_SIZE = 4096;
};



#endif
