// Written by Michael Feeney, Fanshawe College, 2010-2019
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.
//
#ifndef _IGLShaderManager_HG_
#define _IGLShaderManager_HG_

//#include <gl\glext.h>		// OpenGL Extensions
//#include <gl\freeglut.h>
//#include <GL\glew.h>			// Because it complains when we don't include it
//#include <GL\freeglut.h>		// Because of the GLuint
#include "globalOpenGLStuff.h"
#include "ShaderTypes.h"
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

class IGLShaderManager
{
public:
	virtual ~IGLShaderManager() {};

	virtual void SetBaseFilePath( std::string baseFilePath ) = 0;
	virtual std::string GetBaseFilePath(void) = 0;

	virtual bool CreateShaderProgramFromSource( CShaderProgramDescription &shaderProgDescription ) = 0;
	virtual bool CreateShaderProgramFromFile( CShaderProgramDescription &shaderProgDescription ) = 0;

	virtual bool CreateShaderProgramsFromSources( std::vector<CShaderProgramDescription> &shaderProgDescriptions ) = 0;
	virtual bool CreateShaderProgramsFromFiles( std::vector<CShaderProgramDescription> &shaderProgDescriptions ) = 0;

	// Updated: Normally, if you pass the same ID or name, the shader isn't changed
	virtual bool UseShaderProgram( GLuint shaderID, bool bForceShaderProgramSet = false ) = 0;
	virtual bool UseShaderProgram( std::string shaderName, bool bForceShaderProgramSet = false ) = 0;
	// Destroys everything (called when program exits or you are deleting all shaders)
	virtual bool ShutDown(void) = 0;

	// Returns zero (0) and blank string if no valid shader exists
	virtual GLuint GetCurrentShaderID(void) = 0;
	virtual GLuint GetLastShaderID(void) = 0;
	virtual std::string GetCurrentShaderName(void) = 0;
	virtual std::string GetLastShaderName(void) = 0;

	// Returns false if the shader isn't present 
	// NOTE: Used shaderID unless ID == 0, then tries with the shader name
	virtual bool GetShaderProgramInfo( CShaderProgramDescription &shaderProgDescription ) = 0;

	// Uses glGetActiveAttrib and glGetActiveUniform to determine the active variables in use in the shader
	// BIG FAT NOTE: This is only the "active" ones - many times, they are profiled out by the compiler
	virtual bool LoadActiveShaderVariables( GLuint shaderID ) = 0;
	virtual bool LoadActiveShaderVariables( std::string shaderName ) = 0;
	//
	// Returns zero (0) if not found (shader ID can't be zero in OpenGL)
	virtual GLint GetShaderIDFromName( std::string shaderName ) = 0;


	virtual bool DoesShaderExist( GLuint shaderProgramID ) = 0;
	virtual bool DoesShaderExist( std::string shaderName ) = 0;


	virtual bool IsShaderOK( GLuint shaderProgramID ) = 0;
	virtual bool IsShaderOK( std::string shaderName ) = 0;

	virtual std::string GetLastError( void ) = 0;
	virtual bool bIsErrorTextPresent(void) = 0;

	//virtual bool DeleteShader( GLuint shaderProgramID, int &error ) = 0;
	//virtual bool DeleteShader( std::string shaderName, int &error ) = 0;
	////
	////
	//virtual bool LoadAttributeUniform( std::string name ) = 0;
	//virtual bool LoadAttributeVarying( std::string name ) = 0;

	// This creates or uses a buffer for a named uniform block within a shader.
	// Note: If no block exists, one will be created an assigned.
	//       If a block already exists, it will assigned to the shader.
	//       The first block created is the 'referece' block; any subsequent
	//        named blocks must match this one (though the checking is not super detailed). 
	virtual bool InitBufferFromNamedUniformBlock( GLuint shaderProgramID, const std::string &uniformBlockName ) = 0;
	virtual bool InitBufferFromNamedUniformBlock( const std::string &shaderProgramName, const std::string &uniformBlockName ) = 0;
	// Creates all the buffers for all the named uniform blocks for the particular shader
	virtual bool InitBuffersFromUniformBlocks( GLuint shaderProgramID ) = 0;
	virtual bool InitBuffersFromUniformBlocks( const std::string &shaderProgramName ) = 0;
	// Should this be in the shader object?
	// This copies the client side buffer to the shader binding point.
	// Note: This does no checking to see if the information is valid, just that the buffer exists and is in use
	virtual bool UpdateNamedBlockFromBuffer( GLuint shaderProgramID, const std::string &uniformBlockName ) = 0;
	virtual bool UpdateNamedBlockFromBuffer( const std::string &shaderProgramName, const std::string &uniformBlockName ) = 0;

	//virtual bool UpdateNamedUniformBlockToShader( std::string uniformBlockName ) = 0;

	/*************************************************************
		After this is the are endless repeated variations of the 
		varying and uniform variable accessors...
	*************************************************************/
	virtual bool IsUniformInNamedBlock( std::string shaderName, std::string uniformName ) = 0;
	virtual bool IsUniformInNamedBlock( GLuint shaderID, std::string uniformName ) = 0;

	//      _ _   _      _  __               ___  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ / \ \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \| |>  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|_/_/\_\
	//|___/                                         
	// glUniform1f...
	virtual bool SetUniformVar1f( const std::string &shaderName, const std::string &varName, GLfloat value ) = 0;			
	virtual bool SetUniformVar1f( GLuint shaderID, const std::string &varName, GLfloat value ) = 0;	
	virtual bool SetUniformVar1fNamedBlock( const std::string &shaderName, const std::string &varName, GLfloat value ) = 0;
	virtual bool SetUniformVar1fNamedBlock( GLuint shaderID, const std::string &varName, GLfloat value ) = 0;
	// glUniform1i...
	virtual bool SetUniformVar1i( std::string shaderName, std::string varName, GLint value ) = 0;
	virtual bool SetUniformVar1i( GLuint shaderID, std::string varName, GLint value ) = 0;
	// glUniform1ui... (OpenGL 3.0)
	virtual bool SetUniformVar1ui( std::string shaderName, std::string varName, GLint value ) = 0;	// OpenGL 3.0
	virtual bool SetUniformVar1ui( GLuint shaderID, std::string varName, GLint value ) = 0;			// OpenGL 3.0

	 //      _ _   _      _  __               _____  __
	 // __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |_  ) \/ /
	 /// _` | | |_| | ' \| |  _/ _ \ '_| '  \ / / >  < 
	 //\__, |_|\___/|_||_|_|_| \___/_| |_|_|_/___/_/\_\
	 //|___/                                           
	// glUniform2f
	virtual bool SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2 ) = 0;
	virtual bool SetUniformVar2f( std::string shaderName, std::string varName, GLfloat v[2] ) = 0;
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2 ) = 0;
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, GLfloat v[2] ) = 0;
	virtual bool SetUniformVar2f( GLuint shaderID, std::string varName, glm::vec2 v2 ) = 0;
	// glUniform2i
	virtual bool SetUniformVar2i( std::string shaderName, std::string varName, GLint v1, GLint v2 ) = 0;
	virtual bool SetUniformVar2i( std::string shaderName, std::string varName, GLint v[2] ) = 0;
	virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, GLint v1, GLint v2 ) = 0;
	virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, GLint v[2] ) = 0;
	//virtual bool SetUniformVar2i( GLuint shaderID, std::string varName, glm::ivec2 v2 ) = 0;
	// glUniform2ui
	virtual bool SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2 ) = 0;	// OpenGL 3.0
	virtual bool SetUniformVar2ui( std::string shaderName, std::string varName, GLuint v[2] ) = 0;	// OpenGL 3.0
	virtual bool SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2 ) = 0;		// OpenGL 3.0
	virtual bool SetUniformVar2ui( GLuint shaderID, std::string varName, GLuint v[2] ) = 0;		// OpenGL 3.0
	//virtual bool SetUniformVar2ui( GLuint shaderID, std::string varName, glm::uivec2 v2 ) = 0;

	
	//      _ _   _      _  __               ______  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __ |__ /\ \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \ |_ \ >  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_|___//_/\_\
	//|___/                                            
	// glUniform3f
	virtual bool SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 ) = 0;
	virtual bool SetUniformVar3f( std::string shaderName, std::string varName, GLfloat v[3] ) = 0;
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3 ) = 0;
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, GLfloat [3] ) = 0;
	virtual bool SetUniformVar3f( GLuint shaderID, std::string varName, glm::vec3 v3 ) = 0;

	// glUniform3i
	virtual bool SetUniformVar3i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3 ) = 0;
	virtual bool SetUniformVar3i( std::string shaderName, std::string varName, GLint v[3] ) = 0;
	virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3 ) = 0;
	virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, GLint v[3] ) = 0;
	//virtual bool SetUniformVar3i( GLuint shaderID, std::string varName, glm::ivec3 v3 ) = 0;
	// glUniform3ui
	virtual bool SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3 ) = 0;	// OpenGL 3.0
	virtual bool SetUniformVar3ui( std::string shaderName, std::string varName, GLuint v[3] ) = 0;	// OpenGL 3.0
	virtual bool SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3 ) = 0;			// OpenGL 3.0
	virtual bool SetUniformVar3ui( GLuint shaderID, std::string varName, GLuint v[3] ) = 0;			// OpenGL 3.0
	//virtual bool SetUniformVar3ui( GLuint shaderID, std::string varName, glm::uivec3 v3 ) = 0;


	//      _ _   _      _  __              _ ___  __
	// __ _| | | | |_ _ (_)/ _|___ _ _ _ __| | \ \/ /
	/// _` | | |_| | ' \| |  _/ _ \ '_| '  \_  _>  < 
	//\__, |_|\___/|_||_|_|_| \___/_| |_|_|_||_/_/\_\
	//|___/                                          
	// glUniform4f
	virtual bool SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 ) = 0;
	virtual bool SetUniformVar4f( std::string shaderName, std::string varName, GLfloat v[4] ) = 0;
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4 ) = 0;
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, GLfloat v[4] ) = 0;
	virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, glm::vec4 v4 ) = 0;

	// glUniform4i
	virtual bool SetUniformVar4i( std::string shaderName, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 ) = 0;
	virtual bool SetUniformVar4i( std::string shaderName, std::string varName, GLint v[4] ) = 0;
	virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, GLint v1, GLint v2, GLint v3, GLint v4 ) = 0;
	virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, GLint v[4] ) = 0;
	//virtual bool SetUniformVar4i( GLuint shaderID, std::string varName, glm::ivec4 v4 ) = 0;

	// glUniform4ui
	virtual bool SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 ) = 0;		// OpenGL 3.0
	virtual bool SetUniformVar4ui( std::string shaderName, std::string varName, GLuint v[4] ) = 0;		// OpenGL 3.0
	virtual bool SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v1, GLuint v2, GLuint v3, GLuint v4 ) = 0;			// OpenGL 3.0
	virtual bool SetUniformVar4ui( GLuint shaderID, std::string varName, GLuint v[4] ) = 0;			// OpenGL 3.0
	//virtual bool SetUniformVar4f( GLuint shaderID, std::string varName, glm::uivec4 v4 ) = 0;

	virtual bool SetUniformMatrix4fv( std::string shaderName, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value ) = 0;
	virtual bool SetUniformMatrix4fv( GLuint shaderID, std::string varName, GLsizei count, GLboolean transpose, const GLfloat* value ) = 0;
	virtual bool SetUniformMatrixGLM_Mat4x4( GLuint shaderID, std::string varName, GLboolean transpose, const glm::mat4x4 &value ) = 0;

	//PFNGLUNIFORM2IPROC				glUniform2i = 0;
	//PFNGLUNIFORM2UIPROC				glUniform2ui = 0;
};

#endif
