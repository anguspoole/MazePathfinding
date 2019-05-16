#ifndef _CShaderDescription_HG_
#define _CShaderDescription_HG_
//
// Written by Michael Feeney, Fanshawe College, (c) 2010-2019, mfeeney(at)fanshawec.ca
// For the 'Game Development - Advanced Programming' curriculum:  www.fanshawec.ca/en/GDP1
// It may be distributed under the terms of the MIT License: http://opensource.org/licenses/MIT
// Use this code at your own risk. It is indented only as a learning aid.
//


//#include <gl\glext.h>		// OpenGL Extensions
#include "globalOpenGLStuff.h"
#include <string>
#include <vector>
#include <map>


namespace GLSHADERTYPES
{
	enum enumShaderType
	{
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER,
		TESSELLATION_CONTROL_SHADER,
		TESSELLATION_EVALUATION_SHADER,
		COMPUTE_SHADER,
		UNKNOWN
	};
}

// Attribute types (they are type GLenum):
// (Note: There is no GL_UNKNOWN or something like that, so I chose "GL_ID_UNKNOWN" to
//		  indicate an attribute of unknown type.)
//
//	GL_FLOAT,			
//	GL_FLOAT_VEC2,		
//	GL_FLOAT_VEC3,		
//	GL_FLOAT_VEC4, 
//	GL_FLOAT_MAT2, 
//	GL_FLOAT_MAT3, 
//	GL_FLOAT_MAT4, 
//	GL_FLOAT_MAT2x3,		// OpenGL 2.1 only
//	GL_FLOAT_MAT2x4,		// OpenGL 2.1 only
//	GL_FLOAT_MAT3x2,		// OpenGL 2.1 only 
//	GL_FLOAT_MAT3x4,		// OpenGL 2.1 only 
//	GL_FLOAT_MAT4x2,		// OpenGL 2.1 only
//	GL_FLOAT_MAT4x3			// OpenGL 2.1 only
//
//	GL_ID_UNKNOWN
//
class CShaderAttributeDescription
{
public:
	CShaderAttributeDescription();
	std::string programName;
	GLuint		programID;
	std::string name;
	GLuint		index;
	GLenum		type;
	std::string typeAsString;		// English word for GLenum type...
	GLint		size;		// size based on type
	void Clear( void );
};

class CShaderUniformDescription
{
public:
	CShaderUniformDescription();
	std::string programName;
	GLuint		programID;
	std::string name;
	GLuint		index;		// Different from "location": get with glGetActiveUniform() 
	GLint		location;	// Note that this is different from the "index": get with glGetUniformLocation()
	GLenum		type;
	std::string typeAsString;		// English word for GLenum type...
	GLint		size;				// size based on type	(GL_UNIFORM_SIZE)
	// Added
	GLint		namedBlockIndex;			// -1 if in default block (GL_UNIFORM_BLOCK_INDEX)
	std::string blockName;			// 
	GLint		blockOffsetInBytes;		// GL_UNIFORM_OFFSET
	GLint		blockOffsetInFloats;
	bool		bIsInDefaultBlock;
	GLint		nameLength;			// GL_UNIFORM_NAME_LENGTH
	GLint		arrayStride;		// GL_UNIFORM_ARRAY_STRIDE (-1 if not an array)
	bool		bIsAnArray;			// false by default (see arrayStride, above)
	GLint		matrixStride;		// GL_UNIFORM_MATRIX_STRIDE (-1 if default block)
	bool		bIsRowMajor;		// GL_UNIFORM_IS_ROW_MAJOR = 1 means row-major
									// -1 if col-major in default block, or non-matrix
	bool		bIsMatrix;			// false by default
	// 
	void Clear(void);
};

// Added
class CShaderUniformBlockDescription
{
public:
	CShaderUniformBlockDescription();
	std::string		programName;
	GLuint			programID;
	std::string		name;
	GLint			nameLength;			// GL_UNIFORM_BLOCK_NAME_LENGTH?
	GLint			bindingPoint;		// GL_UNIFORM_BLOCK_BINDING
	GLint			dataSize;			// GL_UNIFORM_BLOCK_DATA_SIZE
	GLint			numberOfActiveUniforms;		// GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS;
	std::vector< GLint > vecUniformVarIndices;	// From GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES;
	
	bool bReferencedFromVertexShader;
	bool bReferencedFromTessControlShader;
	bool bReferencedFromTessEvaluationShader;
	bool bReferencedFromGeometryShader;
	bool bReferencedFromFragmentShader;
	bool bReferencedFromComputeShader;

	void Clear(void);
};

class CShaderDescription
{
public:
	CShaderDescription();
	CShaderDescription( std::string name, GLSHADERTYPES::enumShaderType shaderType, 
						std::string source, std::string fileName );
	~CShaderDescription();
	GLuint							ID;
	std::string						name;
	GLSHADERTYPES::enumShaderType	type;	// VERTEX_SHADER, FRAGMENT_SHADER, or GEOMETRY_SHADER
	std::string						source;
	std::string						filename;
	std::vector< std::string >		vecShaderErrors;
	std::string getErrorString( void );
	bool							bIsOK;		// Compiled without errors
};

class CShaderProgramDescription
{
public:
	CShaderProgramDescription();
	CShaderProgramDescription( std::string shaderProgName, 
							   CShaderDescription vShader, CShaderDescription fShader );
	CShaderProgramDescription( std::string shaderProgName, 
							   CShaderDescription vShader, CShaderDescription fShader, CShaderDescription gShader );
	CShaderProgramDescription( std::string shaderProgName, 
							   CShaderDescription vShader, CShaderDescription fShader, CShaderDescription gShader, CShaderDescription tShader );
	// TODO: 
	// * Add constructor that takes the CShaderDescription and processes it by contained type
	// * Add constructor that takes a vector of CShaderDescriptions and processes them by contained type
	~CShaderProgramDescription();
	GLuint						ID;
	std::string					name;
	CShaderDescription			vShader;	// Vertex shader
	CShaderDescription			fShader;	// Fragment shader
	CShaderDescription			gShader;	// Geometry shader
	CShaderDescription			tEvalShader;	// Tessellation Evaluation shader
	CShaderDescription			tContShader;	// Tessellation Control shader
	CShaderDescription			cShader;	// Compute shader (if compute exists, no other shaders can be present)
	// These store the attribute and uniform variables from the shader (after link)
	//GLuint getUniformVarIDFromName( std::string variableName );
	GLuint getUniformLocationFromName( std::string variableName );	// In default block
	GLuint getUniformIndexFromName( std::string variableName );		// In named block
	std::map< std::string /*attribName*/, CShaderAttributeDescription > mapAttributesByName;
	//std::map< std::string /*uniformName*/, CShaderUniformDescription > mapShaderUniforms;

	// The uniforms listed by OpenGL index, rather than name
	// The vector contains the uniform info and the map is a lookup to that
	std::map< std::string /*uniformName*/, GLuint /*index*/ > mapUniformVarName_to_Location;
	std::map< std::string /*uniformName*/, GLuint /*index*/ > mapUniformName_to_Index;
	std::vector< CShaderUniformDescription > vecUniformVariables;
	bool GetUniformVarDesc( GLuint index, CShaderUniformDescription &UniformVarDesc );
	bool GetUniformVarDesc( const std::string &name, CShaderUniformDescription &UniformVarDesc );

	std::vector< CShaderUniformBlockDescription > vecNamedUniformBlocks;		// Also ID to name
	std::map< std::string /*block name*/, GLint /*blockID*/ > mapNamedUniformBlockName_to_Index;
	bool GetNamedBlock( GLuint index, CShaderUniformBlockDescription &NamedBlockDesc );
	bool GetNamedBlock( const std::string &name, CShaderUniformBlockDescription &NamedBlockDesc );

	// Used to print out a summary of what's in the shaders. Only really useful for debugging.
	std::string getActiveAttribsPrintableSummary(bool bSortByIndex);
	std::string getActiveUnifomsPrintableSummary(bool bSortByIndex);
	std::string getActiveNamedUniformBlockPrintableSummary(bool bSortByIndex);
	//
	std::vector< std::string >	vecErrors;
	std::string getErrorString( void );
	bool						bIsOK;		// Compiled, linked, and ready to go.
private:
	// Predicate functions used for sorting 
	static bool m_pfSortAttribByIndex( const CShaderAttributeDescription &a1, const CShaderAttributeDescription &a2 );
	static bool m_pfSortUniformByIndex( const CShaderUniformDescription &u1, const CShaderUniformDescription &u2 );

};

#endif
