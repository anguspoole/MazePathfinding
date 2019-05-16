// Written by Michael Feeney, Fanshawe College, 2010-2019
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the General Public License:
// http://www.fsf.org/licenses/gpl.html
// Use this code at your own risk. It is indented only as a learning aid.
//
#include "CGLShaderManager.h"
#include <sstream>
#include <fstream>
//#include "OpenGLExt.h"
#include <algorithm>	

//#include "../CError/COpenGLError.h"
#include "Error/COpenGLError.h"

CGLShaderManager::CGLShaderManager()
{
	this->m_currentShaderID = 0;
	this->m_lastShaderID = 0;
	// Figure out how many uniform buffer binding we can use 
	this->m_maxUniformBindings = this->GetMaxUniformBindings();
	this->m_vecUniformBufferBindings.reserve( this->m_maxUniformBindings );
	for ( GLuint index = 0; index != this->m_maxUniformBindings; index++ )
	{
		CUniformBlockBuffer tempBlock;
		tempBlock.bindingPoint = index;
		this->m_vecUniformBufferBindings.push_back( tempBlock );
	}
	return;
}

CGLShaderManager::~CGLShaderManager()
{
	return;
}
//

void CGLShaderManager::SetBaseFilePath( std::string baseFilePath )
{
	this->m_baseFilePath = baseFilePath;
	return;
}

std::string CGLShaderManager::GetBaseFilePath(void)
{
	return this->m_baseFilePath;
}

//bool CGLShaderManager::m_compileShaderFromSource_OLD_DONT_USE( CShaderDescription &shader, std::string &error )
//{
//	error = "";
//
//	const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive
//
//	unsigned int numberOfLines = static_cast<unsigned int>(shader.source.size());
//
//	// This is an array of pointers to strings. aka the lines of source
//	char** arraySource = new char*[numberOfLines];
//	// Clear array to all zeros (i.e. '\0' or null terminator)
//	memset( arraySource, 0, numberOfLines );	
//
//	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
//	{
//		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
//		// Create an array of chars for each line
//		arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
//		memset( arraySource[indexLine], 0, numCharacters + 2 );
//
//		// Copy line of source into array
//		for ( unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++ )
//		{
//			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
//		}//for ( unsigned int indexChar = 0...
//		
//		// At a '\0' at end (just in case)
//		arraySource[indexLine][numCharacters + 0] = '\n';
//		arraySource[indexLine][numCharacters + 1] = '\0';
//
//		// Or you could use the (unsecure) strcpy (string copy)
////		strcpy( arraySource[indexLine], shader.vecSource[indexLine].c_str() );
//
//		// Or the "secure" version, where you specify the number of chars to copy
////		strcpy_s( arraySource[indexLine], 
////				  strlen( shader.vecSource[indexLine].c_str() + 1 ),	// Number of char in copy + 0 terminator
////				  shader.vecSource[indexLine].c_str() );
//
//	}//for ( unsigned int indexLine = 0...
//
////******************************************************************************************
////	// Print it out (for debug)
////	std::cout << "Source (from char** array):" << std::endl;
////	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
////	{
////		std::cout << "(" << indexLine << "):\t" << arraySource[indexLine] << std::endl;
////	}
////	std::cout << "END" << std::endl;
////******************************************************************************************
//
//	//const char* tempVertChar = "Hey there;";
//
//	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
//    glCompileShader(shader.ID); 
//
//	// Get rid of the temp source "c" style array
//	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
//	{	// Delete this line
//		delete [] arraySource[indexLine];
//	}
//	// And delete the original char** array
//	delete [] arraySource;
//
//	// Did it work? 
//	std::string errorText = "";
//	if ( this->m_wasThereACompileError( shader.ID, errorText ) )
//	{
//		std::stringstream ssError;
//		ssError << shader.getShaderTypeString();
//		ssError << " compile error: ";
//		ssError << errorText;
//		error = ssError.str();
//		return false;
//	}
//
//	return true;
//}


// Returns empty string if no error
// returns false if no error
bool CGLShaderManager::m_wasThereACompileError( unsigned int shaderID, 
											  std::string &errorText )
{
	errorText = "";	// No error

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_LastError.append("\n");
		this->m_LastError.append( errorText );


		delete [] pLogText;	// Oops

		return true;	// There WAS an error
	}
	return false; // There WASN'T an error
}

bool CGLShaderManager::m_wasThereALinkError( unsigned int programID, std::string &errorText )
{
	errorText = "";	// No error

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if(wasError == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_LastError.append("\n");
		this->m_LastError.append( errorText );


		delete [] pLogText;	

		// There WAS an error
		return true;	
	}
	
	// There WASN'T an error
	return false; 
}

// If no vertex shader source, return false...
// Fragment and geometry shader is optional (i.e. no source = warning)
bool CGLShaderManager::CreateShaderProgramFromSource( CShaderProgramDescription &shaderProgDescription )
{
	if ( shaderProgDescription.vShader.source == "" )
	{	// No vertex source...
		std::stringstream ss;
		ss << "error: vertex shader '" << shaderProgDescription.vShader.name << "' has no source.";
		shaderProgDescription.vShader.vecShaderErrors.push_back( ss.str() );
		return false;	
	}

	// See if we've already added this shader... (check the name)
	if ( this->m_mapShaderName_to_ID.find( shaderProgDescription.name ) != this->m_mapShaderName_to_ID.end() )
	{	// We already have this shader loaded...
		

	}
		this->m_mapShaderName_to_ID[ shaderProgDescription.name ] = shaderProgDescription.ID;


	//
	shaderProgDescription.ID = ::glCreateProgram();
	
	bool bAllShadersCompiled = true;	// Assume it's all good


	//
	//__   __       _             ___ _            _         
	//\ \ / /__ _ _| |_ _____ __ / __| |_  __ _ __| |___ _ _ 
	// \ V / -_) '_|  _/ -_) \ / \__ \ ' \/ _` / _` / -_) '_|
	//  \_/\___|_|  \__\___/_\_\ |___/_||_\__,_\__,_\___|_|  
	//
	// Attempt to create and compile the vertex shader...
	shaderProgDescription.vShader.ID = ::glCreateShader( GL_VERTEX_SHADER );

	// Attach the shader(s) to the 'program' 
	::glAttachShader( shaderProgDescription.ID, shaderProgDescription.vShader.ID );
	// Get source as char array....
	GLcharARB* vShaderSource[1];
	vShaderSource[0] = (char *)shaderProgDescription.vShader.source.c_str();
	//...to set source...
	::glShaderSource( shaderProgDescription.vShader.ID,		// Handle to shader
					  1,					// Number of elements in string array (if using one big string, it's 1)
					  (const GLcharARB**) vShaderSource,	// Pointer to shader source (char array)
					  NULL);				// Length of array (0 or NULL if null terminated)
	// ...and compile.
	::glCompileShader( shaderProgDescription.vShader.ID );
	//
	// Did compile work?
	//GLint statusOK; 
	//::glGetObjectParameterivARB( shaderProgDescription.vShader.ID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
	std::string errorText; 
	if ( this->m_wasThereACompileError( shaderProgDescription.vShader.ID, errorText ) )
	{
		shaderProgDescription.vShader.vecShaderErrors.push_back("Error: Could not compile vertex shader.");
		// This gets the 'last' error message for that shader (if there was one)
//		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//		glGetInfoLogARB( shaderProgDescription.vShader.ID , GL_INFO_LOG_LENGTH, NULL, infoLog );
//		std::stringstream ss;
//		ss << infoLog << std::endl;
		shaderProgDescription.vShader.vecShaderErrors.push_back( errorText );
		this->m_LastError += "\n";
		this->m_LastError += errorText;
		// indicate a shader didn't compile (to prevent link, later)
		bAllShadersCompiled = false;
	}
	else
	{
		shaderProgDescription.vShader.type = GLSHADERTYPES::VERTEX_SHADER;
		shaderProgDescription.vShader.bIsOK = true;
	}


	// ___                            _     ___ _            _         
	//| __| _ __ _ __ _ _ __  ___ _ _| |_  / __| |_  __ _ __| |___ _ _ 
	//| _| '_/ _` / _` | '  \/ -_) ' \  _| \__ \ ' \/ _` / _` / -_) '_|
	//|_||_| \__,_\__, |_|_|_\___|_||_\__| |___/_||_\__,_\__,_\___|_|  
	//            |___/                                                
	//
	if ( shaderProgDescription.fShader.source != "" )
	{	//
		shaderProgDescription.fShader.ID = ::glCreateShader( GL_FRAGMENT_SHADER );

		// Attach the shader(s) to the 'program' 
		::glAttachShader( shaderProgDescription.ID, shaderProgDescription.fShader.ID );
		// Get source as char array....
		GLcharARB* fShaderSource[1];
		fShaderSource[0] = (char *)shaderProgDescription.fShader.source.c_str();
		//...to set source...
		::glShaderSource( shaderProgDescription.fShader.ID,		// Handle to shader
						  1,					// Number of elements in string array (if using one big string, it's 1)
						  (const GLcharARB**) fShaderSource,	// Pointer to shader source (char array)
						  NULL);				// Length of array (0 or NULL if null terminated)
		// ...and compile.
		::glCompileShader( shaderProgDescription.fShader.ID );
		//
		// Did compile work?
//		GLint statusOK; 
//		::glGetObjectParameterivARB( shaderProgDescription.fShader.ID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
//		if ( !statusOK )
		std::string errorText; 
		if ( this->m_wasThereACompileError( shaderProgDescription.fShader.ID, errorText ) )
		{
			shaderProgDescription.fShader.vecShaderErrors.push_back("Error: Could not compile fragment shader.");
			// This gets the 'last' error message for that shader (if there was one)
//			GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//			glGetInfoLogARB( shaderProgDescription.fShader.ID , GL_INFO_LOG_LENGTH, NULL, infoLog );
//			std::stringstream ss;
//			ss << infoLog << std::endl;
			shaderProgDescription.fShader.vecShaderErrors.push_back( errorText );
			// indicate a shader didn't compile (to prevent link, later)
			this->m_LastError += "\n";
			this->m_LastError += errorText;
			bAllShadersCompiled = false;
		}
		else
		{
			shaderProgDescription.fShader.type = GLSHADERTYPES::FRAGMENT_SHADER;
			shaderProgDescription.fShader.bIsOK = true;
		}
	} // if ( shaderProgDescription.fShader.source != "" )

	//  ___                   _              ___ _            _         
	// / __|___ ___ _ __  ___| |_ _ _ _  _  / __| |_  __ _ __| |___ _ _ 
	//| (_ / -_) _ \ '  \/ -_)  _| '_| || | \__ \ ' \/ _` / _` / -_) '_|
	// \___\___\___/_|_|_\___|\__|_|  \_, | |___/_||_\__,_\__,_\___|_|  
	//                                |__/                              
	if ( shaderProgDescription.gShader.source != "" )
	{	//
		shaderProgDescription.gShader.ID = ::glCreateShader( GL_GEOMETRY_SHADER );

		// Attach the shader(s) to the 'program' 
		::glAttachShader( shaderProgDescription.ID, shaderProgDescription.gShader.ID );
		// Get source as char array....
		GLcharARB* gShaderSource[1];
		gShaderSource[0] = (char *)shaderProgDescription.gShader.source.c_str();
		//...to set source...
		::glShaderSource( shaderProgDescription.gShader.ID,		// Handle to shader
						  1,					// Number of elements in string array (if using one big string, it's 1)
						  (const GLcharARB**) gShaderSource,	// Pointer to shader source (char array)
						  NULL);				// Length of array (0 or NULL if null terminated)
		// ...and compile.
		::glCompileShader( shaderProgDescription.gShader.ID );
		//
		// Did compile work?
//		GLint statusOK; 
//		::glGetObjectParameterivARB( shaderProgDescription.gShader.ID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
//		if ( !statusOK )
		std::string errorText; 
		if ( this->m_wasThereACompileError( shaderProgDescription.gShader.ID, errorText ) )
		{
			shaderProgDescription.gShader.vecShaderErrors.push_back("Error: Could not compile geometry shader.");
			// This gets the 'last' error message for that shader (if there was one)
//			GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//			glGetInfoLogARB( shaderProgDescription.gShader.ID , GL_INFO_LOG_LENGTH, NULL, infoLog );
//			std::stringstream ss;
//			ss << infoLog << std::endl;
			shaderProgDescription.gShader.vecShaderErrors.push_back( errorText );
			// indicate a shader didn't compile (to prevent link, later)
			this->m_LastError += "\n";
			this->m_LastError += errorText;
			bAllShadersCompiled = false;
		}
		else
		{
			shaderProgDescription.gShader.type = GLSHADERTYPES::GEOMETRY_SHADER;
			shaderProgDescription.gShader.bIsOK = true;
		}
	} // if ( shaderProgDescription.fShader.source != "" )


 // _____               _ _      _   _             ___         _           _   ___ _            _         
 //|_   _|__ ______ ___| | |__ _| |_(_)___ _ _    / __|___ _ _| |_ _ _ ___| | / __| |_  __ _ __| |___ _ _ 
 //  | |/ -_|_-<_-</ -_) | / _` |  _| / _ \ ' \  | (__/ _ \ ' \  _| '_/ _ \ | \__ \ ' \/ _` / _` / -_) '_|
 //  |_|\___/__/__/\___|_|_\__,_|\__|_\___/_||_|  \___\___/_||_\__|_| \___/_| |___/_||_\__,_\__,_\___|_|  
	if ( shaderProgDescription.tContShader.source != "" )
	{	//
		shaderProgDescription.tContShader.ID = ::glCreateShader( GL_TESS_CONTROL_SHADER );

		// Attach the shader(s) to the 'program' 
		::glAttachShader( shaderProgDescription.ID, shaderProgDescription.tContShader.ID );
		// Get source as char array....
		GLcharARB* tContShaderSource[1];
		tContShaderSource[0] = (char *)shaderProgDescription.tContShader.source.c_str();
		//...to set source...
		::glShaderSource( shaderProgDescription.tContShader.ID,		// Handle to shader
						  1,					// Number of elements in string array (if using one big string, it's 1)
						  (const GLcharARB**) tContShaderSource,	// Pointer to shader source (char array)
						  NULL);				// Length of array (0 or NULL if null terminated)
		// ...and compile.
		::glCompileShader( shaderProgDescription.tContShader.ID );
		//
		// Did compile work?
//		GLint statusOK; 
//		::glGetObjectParameterivARB( shaderProgDescription.tContShader.ID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
//		if ( !statusOK )
		std::string errorText; 
		if ( this->m_wasThereACompileError( shaderProgDescription.tContShader.ID, errorText ) )
		{
			shaderProgDescription.tContShader.vecShaderErrors.push_back("Error: Could not compile tessellation control shader.");
			// This gets the 'last' error message for that shader (if there was one)
//			GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//			glGetInfoLogARB( shaderProgDescription.tContShader.ID , GL_INFO_LOG_LENGTH, NULL, infoLog );
//			std::stringstream ss;
//			ss << infoLog << std::endl;
			shaderProgDescription.tContShader.vecShaderErrors.push_back( errorText );
			this->m_LastError += "\n";
			this->m_LastError += errorText;
			// indicate a shader didn't compile (to prevent link, later)
			bAllShadersCompiled = false;
		}
		else
		{
			shaderProgDescription.tContShader.type = GLSHADERTYPES::TESSELLATION_CONTROL_SHADER;
			shaderProgDescription.tContShader.bIsOK = true;
		}
	} // if ( shaderProgDescription.fShader.source != "" )                                                                                                        


 // _____               _ _      _   _            ___          _           _   _            ___ _            _         
 //|_   _|__ ______ ___| | |__ _| |_(_)___ _ _   | __|_ ____ _| |_  _ __ _| |_(_)___ _ _   / __| |_  __ _ __| |___ _ _ 
 //  | |/ -_|_-<_-</ -_) | / _` |  _| / _ \ ' \  | _|\ V / _` | | || / _` |  _| / _ \ ' \  \__ \ ' \/ _` / _` / -_) '_|
 //  |_|\___/__/__/\___|_|_\__,_|\__|_\___/_||_| |___|\_/\__,_|_|\_,_\__,_|\__|_\___/_||_| |___/_||_\__,_\__,_\___|_|  
 	if ( shaderProgDescription.tEvalShader.source != "" )
	{	//
		shaderProgDescription.tEvalShader.ID = ::glCreateShader( GL_TESS_EVALUATION_SHADER );

		// Attach the shader(s) to the 'program' 
		::glAttachShader( shaderProgDescription.ID, shaderProgDescription.tEvalShader.ID );
		// Get source as char array....
		GLcharARB* tEvalShaderSource[1];
		tEvalShaderSource[0] = (char *)shaderProgDescription.tEvalShader.source.c_str();
		//...to set source...
		::glShaderSource( shaderProgDescription.tEvalShader.ID,		// Handle to shader
						  1,					// Number of elements in string array (if using one big string, it's 1)
						  (const GLcharARB**) tEvalShaderSource,	// Pointer to shader source (char array)
						  NULL);				// Length of array (0 or NULL if null terminated)
		// ...and compile.
		::glCompileShader( shaderProgDescription.tEvalShader.ID );
		//
		// Did compile work?
//		GLint statusOK; 
//		::glGetObjectParameterivARB( shaderProgDescription.tEvalShader.ID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
//		if ( !statusOK )
		std::string errorText; 
		if ( this->m_wasThereACompileError( shaderProgDescription.tEvalShader.ID, errorText ) )
		{
			shaderProgDescription.tEvalShader.vecShaderErrors.push_back("Error: Could not compile tessellation evaulation shader.");
			// This gets the 'last' error message for that shader (if there was one)
//			GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//			glGetInfoLogARB( shaderProgDescription.tEvalShader.ID , GL_INFO_LOG_LENGTH, NULL, infoLog );
//			std::stringstream ss;
//			ss << infoLog << std::endl;
			shaderProgDescription.tEvalShader.vecShaderErrors.push_back( errorText );
			// indicate a shader didn't compile (to prevent link, later)
			this->m_LastError += "\n";
			this->m_LastError += errorText;
			bAllShadersCompiled = false;
		}
		else
		{
			shaderProgDescription.tEvalShader.type = GLSHADERTYPES::TESSELLATION_EVALUATION_SHADER;
			shaderProgDescription.tEvalShader.bIsOK = true;
		}
	} // if ( shaderProgDescription.fShader.source != "" )                                                                                                        
                                                                                                                    
	// Did every shader compile?
	if ( ! bAllShadersCompiled )
	{	// Nope. return to prevent link
		return false;
	}

	// Now link the program...
//	::glLinkProgramARB( shaderProgDescription.ID );

	// Note: all the shaders are already attached (see above)
	glLinkProgram( shaderProgDescription.ID );


	// NOTE: This can still work, for instance if you have no shaders at all, but 
	//	will fail if there has been a complie error...
//	::glGetObjectParameterivARB( shaderProgDescription.ID, GL_LINK_STATUS, &statusOK );
//	if ( !statusOK )
	if ( this->m_wasThereALinkError( shaderProgDescription.ID, errorText ) )
	{
//		std::stringstream ss;
//		ss << "error: could not link shader program." << std::endl;
//		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
//		glGetInfoLogARB( shaderProgDescription.ID, GL_INFO_LOG_LENGTH, NULL, infoLog );
//		ss << infoLog;
		shaderProgDescription.vecErrors.push_back( errorText );
		this->m_LastError += "\n";
		this->m_LastError += errorText;
		return false;
	}

	// Place the shader program into the container of shaders...
	//	( std::map< GLuint, CShaderProgramDescription > )
	this->m_mapShaderProgram[ shaderProgDescription.ID ] = shaderProgDescription;	
	// 
	//	( std::map< std::string, GLuint> m_ShaderName_to_MapID; )
	this->m_mapShaderName_to_ID[ shaderProgDescription.name ] = shaderProgDescription.ID;

	// At this point the shader is good to go...
	shaderProgDescription.bIsOK = true;
	// 
	
	// Now load up the atrribute and uniform variables...
	//	(Note that this 'shouldn't' fail if the compile and link was OK
	if ( !this->LoadActiveShaderVariables( shaderProgDescription.ID ) ) 
	{
		std::stringstream ss;
		ss << "Unexpected fatal error: could not load shader attribute descriptions for shader "
			<< shaderProgDescription.name << ".";
		this->m_LastError = ss.str();
		return false;
	}



	return true;
}

// **************************************************************************************************
// This loads the source from the files, then calls CreateShaderProgramFromSource...
// If there is a problem loading the file, returns false and loads the appropriate 
//	vector of errors with the error text...
// NOTES: 
//	* If vertex shader fails to load, returns false
//	* Fragment and Geometry shader is optional: if no filename specified, skips with warning
//	* If a file name IS specified for the Fragment and-or Geometry shader AND
//	  there is a problem loading it, returns false.
//
bool CGLShaderManager::CreateShaderProgramFromFile( CShaderProgramDescription &shaderProgDescription )
{
	//__   __       _             ___ _            _         
	//\ \ / /__ _ _| |_ _____ __ / __| |_  __ _ __| |___ _ _ 
	// \ V / -_) '_|  _/ -_) \ / \__ \ ' \/ _` / _` / -_) '_|
	//  \_/\___|_|  \__\___/_\_\ |___/_||_\__,_\__,_\___|_|  
	//
	// Attempt to load the vertex shader...
	if ( shaderProgDescription.vShader.filename == "")
	{	// No file name given
		std::stringstream ss;
		ss << "error: vertex shader '" << shaderProgDescription.vShader.name << "' has no filename specified.";
		shaderProgDescription.vShader.vecShaderErrors.push_back( ss.str() );
		return false;
	}
	// Has a file name, so attempt to load it...
	if ( !this->m_LoadShaderFromFile( shaderProgDescription.vShader.filename, shaderProgDescription.vShader.source ) )
	{	// Didn't load
		std::stringstream ss;
		ss << "error: Can't find vertex shader source file '" << shaderProgDescription.vShader.filename << "'";
		shaderProgDescription.vShader.vecShaderErrors.push_back( ss.str() );
		this->m_LastError += "\n";
		this->m_LastError += ss.str();
		return false;
	}

	// NOTE: Fragment and geometry shader is optional... 
	//	If there is no file name specified, then just continue...
	// ___                            _     ___ _            _         
	//| __| _ __ _ __ _ _ __  ___ _ _| |_  / __| |_  __ _ __| |___ _ _ 
	//| _| '_/ _` / _` | '  \/ -_) ' \  _| \__ \ ' \/ _` / _` / -_) '_|
	//|_||_| \__,_\__, |_|_|_\___|_||_\__| |___/_||_\__,_\__,_\___|_|  
	//            |___/                                                
	//
	// Attempt to load the fragment shader...
	if ( shaderProgDescription.fShader.filename == "")
	{	// No file name given - GIVE WARNING (fragment) and continue...
		std::stringstream ss;
		ss << "warning: fragment shader '" << shaderProgDescription.fShader.name << "' has no filename specified.";
		shaderProgDescription.fShader.vecShaderErrors.push_back( ss.str() );
	}
	else
	{	// Has a file name, so attempt to load it...
		if ( !this->m_LoadShaderFromFile( shaderProgDescription.fShader.filename, shaderProgDescription.fShader.source ) )
		{	// Didn't load
			std::stringstream ss;
			ss << "error: Can't find fragment shader source file '" << shaderProgDescription.fShader.filename << "'";
			shaderProgDescription.fShader.vecShaderErrors.push_back( ss.str() );
			this->m_LastError += "\n";
			this->m_LastError += ss.str();
			return false;
		}
	}			
	
	//  ___                   _              ___ _            _         
	// / __|___ ___ _ __  ___| |_ _ _ _  _  / __| |_  __ _ __| |___ _ _ 
	//| (_ / -_) _ \ '  \/ -_)  _| '_| || | \__ \ ' \/ _` / _` / -_) '_|
	// \___\___\___/_|_|_\___|\__|_|  \_, | |___/_||_\__,_\__,_\___|_|  
	//                                |__/                              
	//
	// Attempt to load the geometry shader...
	if ( shaderProgDescription.gShader.filename == "")
	{	// No file name given - GIVE WARNING (geometry) and continue...
		std::stringstream ss;
		ss << "warning: geometry shader '" << shaderProgDescription.gShader.name << "' has no filename specified.";
		shaderProgDescription.gShader.vecShaderErrors.push_back( ss.str() );
	}
	else
	{	// Has a file name, so attempt to load it...
		if ( !this->m_LoadShaderFromFile( shaderProgDescription.gShader.filename, shaderProgDescription.gShader.source ) )
		{	// Didn't load
			std::stringstream ss;
			ss << "error: Can't find geometry shader source file '" << shaderProgDescription.gShader.filename << "'";
			shaderProgDescription.gShader.vecShaderErrors.push_back( ss.str() );
			this->m_LastError += "\n";
			this->m_LastError += ss.str();
			return false;
		}
	}			

 // _____               _ _      _   _             ___         _           _   ___ _            _         
 //|_   _|__ ______ ___| | |__ _| |_(_)___ _ _    / __|___ _ _| |_ _ _ ___| | / __| |_  __ _ __| |___ _ _ 
 //  | |/ -_|_-<_-</ -_) | / _` |  _| / _ \ ' \  | (__/ _ \ ' \  _| '_/ _ \ | \__ \ ' \/ _` / _` / -_) '_|
 //  |_|\___/__/__/\___|_|_\__,_|\__|_\___/_||_|  \___\___/_||_\__|_| \___/_| |___/_||_\__,_\__,_\___|_|  
	// Attempt to load the tessellation control shader...
	if ( shaderProgDescription.tContShader.filename == "")
	{	// No file name given - GIVE WARNING (tessellation control) and continue...
		std::stringstream ss;
		ss << "warning: tessellation control shader '" << shaderProgDescription.tContShader.name << "' has no filename specified.";
		shaderProgDescription.tContShader.vecShaderErrors.push_back( ss.str() );
	}
	else
	{	// Has a file name, so attempt to load it...
		if ( !this->m_LoadShaderFromFile( shaderProgDescription.tContShader.filename, shaderProgDescription.tContShader.source ) )
		{	// Didn't load
			std::stringstream ss;
			ss << "error: Can't find geometry shader source file '" << shaderProgDescription.tContShader.filename << "'";
			shaderProgDescription.tContShader.vecShaderErrors.push_back( ss.str() );
			return false;
		}
	}	
	

 // _____               _ _      _   _            ___          _           _   _            ___ _            _         
 //|_   _|__ ______ ___| | |__ _| |_(_)___ _ _   | __|_ ____ _| |_  _ __ _| |_(_)___ _ _   / __| |_  __ _ __| |___ _ _ 
 //  | |/ -_|_-<_-</ -_) | / _` |  _| / _ \ ' \  | _|\ V / _` | | || / _` |  _| / _ \ ' \  \__ \ ' \/ _` / _` / -_) '_|
 //  |_|\___/__/__/\___|_|_\__,_|\__|_\___/_||_| |___|\_/\__,_|_|\_,_\__,_|\__|_\___/_||_| |___/_||_\__,_\__,_\___|_|  
	// Attempt to load the tessellation control shader...
	if ( shaderProgDescription.tEvalShader.filename == "")
	{	// No file name given - GIVE WARNING (geometry) and continue...
		std::stringstream ss;
		ss << "warning: tessellation evaluation shader '" << shaderProgDescription.tEvalShader.name << "' has no filename specified.";
		shaderProgDescription.tEvalShader.vecShaderErrors.push_back( ss.str() );
	}
	else
	{	// Has a file name, so attempt to load it...
		if ( !this->m_LoadShaderFromFile( shaderProgDescription.tEvalShader.filename, shaderProgDescription.tEvalShader.source ) )
		{	// Didn't load
			std::stringstream ss;
			ss << "error: Can't find geometry shader source file '" << shaderProgDescription.tEvalShader.filename << "'";
			shaderProgDescription.tEvalShader.vecShaderErrors.push_back( ss.str() );
			return false;
		}
	}	

	
	
	// All source loaded...now load the shader...	
	return this->CreateShaderProgramFromSource( shaderProgDescription );
}


bool CGLShaderManager::CreateShaderProgramsFromSources( std::vector<CShaderProgramDescription> &shaderProgDescriptions )
{
	// Assume all the shaders loaded OK.
	bool bAllOK = true;
	for ( std::vector<CShaderProgramDescription>::iterator itShadProg = shaderProgDescriptions.begin();
		  itShadProg != shaderProgDescriptions.end(); itShadProg++ )
	{
		if ( !(this->CreateShaderProgramFromSource( *itShadProg ) ) )
		{	// This one didn't load OK, so set the general flag
			bAllOK = false;
		}
	}
	return bAllOK;
}

bool CGLShaderManager::CreateShaderProgramsFromFiles( std::vector<CShaderProgramDescription> &shaderProgDescriptions )
{
	// Assume all the shaders loaded OK.
	bool bAllOK = true;
	for ( std::vector<CShaderProgramDescription>::iterator itShadProg = shaderProgDescriptions.begin();
		  itShadProg != shaderProgDescriptions.end(); itShadProg++ )
	{
		if ( !(this->CreateShaderProgramFromFile( *itShadProg ) ) )
		{	// This one didn't load OK, so set the general flag
			bAllOK = false;
		}
	}
	return bAllOK;
}


GLuint CGLShaderManager::GetCurrentShaderID(void)
{
	return this->m_currentShaderID;
}

GLuint CGLShaderManager::GetLastShaderID(void)
{
	return this->m_lastShaderID;
}

std::string CGLShaderManager::GetCurrentShaderName(void)
{
	for ( std::map< std::string, GLuint>::iterator itShader = this->m_mapShaderName_to_ID.begin();
		  itShader != this->m_mapShaderName_to_ID.end(); itShader++ )
	{
		if ( itShader->second == this->m_currentShaderID )
		{
			// It's the one
			return itShader->first;
		}
	}
	return "";	// No current shader
}

std::string CGLShaderManager::GetLastShaderName(void)
	{
	for ( std::map< std::string, GLuint>::iterator itShader = this->m_mapShaderName_to_ID.begin();
		  itShader != this->m_mapShaderName_to_ID.end(); itShader++ )
	{
		if ( itShader->second == this->m_lastShaderID )
		{
			// It's the one
			return itShader->first;
		}
	}
	return "";	// No current shader
}

//bool CGLShaderManager::m_LoadShaderFromFile( std::string shaderFileName, std::string &shaderSource )
//{
//	std::ifstream theShaderFile( shaderFileName.c_str() );
//	if ( !theShaderFile.is_open() )
//	{
//		return false;
//	}
//	// File is open
//	std::stringstream ss;
//	std::string tempText;
//	while ( theShaderFile >> tempText )	
//	{
//		ss << tempText << " ";
//	}
//	shaderSource = ss.str();
//	return true;
//}


bool CGLShaderManager::m_LoadShaderFromFile( std::string shaderFileName, std::string &shaderSource )
{
	// Append the base file path to name just for the load
	std::string fileNameWithFullPath = this->m_baseFilePath + shaderFileName;

	//std::ifstream theShaderFile( shaderFileName.c_str() );
	std::ifstream theShaderFile( fileNameWithFullPath );
	if ( !theShaderFile.is_open() )
	{
		return false;
	}
	// File is open
	std::stringstream ss;
	std::string tempText;
	//const int MAX_BUFFERLENGTH = 1024;
	char buffer[CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE];	// Buffer

	while ( ! theShaderFile.eof() )	
	{
		// Clear buffer (likely not needed, but hey, can't hurt...)
		std::fill( buffer, buffer + CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE, ' ');
		theShaderFile.getline( buffer, CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE );
		ss << buffer << " \n";
	}
	shaderSource = ss.str();

	theShaderFile.close();

	return true;
}


bool CGLShaderManager::UseShaderProgram( GLuint shaderID, bool bForceShaderProgramSet )
{
	// If zero (0) is passed, then just set it (i.e. "no" shader
	if ( shaderID == 0 )
	{
		::glUseProgram( 0 );
		this->m_lastShaderID = this->m_currentShaderID;
		this->m_currentShaderID = 0;
		return true;
	}

	// find it
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{
		std::stringstream ss;
		ss << "Could not find shader program #" << shaderID;
		this->m_LastError = ss.str();
		return false;
	}
	//

	// Is this the current shader? 
	if ( ! bForceShaderProgramSet )
	{
		if ( this->m_currentShaderID == shaderID )
		{	// Yes, so don't switch it
			this->m_lastShaderID = this->m_currentShaderID;
			this->m_currentShaderID = shaderID;
			return true;
		}
		// Else the shader ISN'T the same one so we'll continue, anyway
	}

	// It's NOT the same shader (or we are forcing to set it), so switch it
	::glUseProgram( shaderID );
	this->m_lastShaderID = this->m_currentShaderID;
	this->m_currentShaderID = shaderID;

	return true;
}

bool CGLShaderManager::UseShaderProgram( std::string shaderName, bool bForceShaderProgramSet )
{
	// Is it there? 
	if ( this->m_mapShaderName_to_ID.find( shaderName ) == this->m_mapShaderName_to_ID.end() )
	{
		std::stringstream ss;
		ss << "Could not find '" << shaderName << "' shader program.";
		this->m_LastError = ss.str();
		return false;
	}
	GLuint shaderID = this->m_mapShaderName_to_ID[ shaderName ];
	// 
	//::glUseProgram( shaderID );
	this->UseShaderProgram( shaderID, bForceShaderProgramSet );

	return true;
}

std::string CGLShaderManager::GetLastError( void )
{
	std::string tempErrorText = this->m_LastError;
	this->m_LastError = "";
	return tempErrorText;
}

bool CGLShaderManager::bIsErrorTextPresent(void)
{
	if ( this->m_LastError != "" )	{  return true; }
	return false;
}



//bool CGLShaderManager::GetShaderProgramInfo( std::string shaderName,  
//											 CShaderProgramDescription &shaderProgDescription )
//{
//	// Is it there? 
//	if ( this->m_mapShaderName_to_ID.find( shaderName ) == this->m_mapShaderName_to_ID.end() )
//	{
//		std::stringstream ss;
//		ss << "Could not find '" << shaderName << "' shader program.";
//		this->m_LastError = ss.str();
//		return false;
//	}
//	GLuint shaderID = this->m_mapShaderName_to_ID[ shaderName ];
//	shaderProgDescription = this->m_mapShaderProgram[ shaderID ];
//	return true;
//}
//
//bool CGLShaderManager::GetShaderProgramInfo( GLuint shaderID, 
//											 CShaderProgramDescription &shaderProgDescription )
//{
//	// find it
//	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
//	{
//		std::stringstream ss;
//		ss << "Could not find shader program #" << shaderID;
//		this->m_LastError = ss.str();
//		return false;
//	}
//	//
//	shaderProgDescription = this->m_mapShaderProgram[ shaderID ];
//
//	return true;
//}

bool CGLShaderManager::IsShaderOK( GLuint shaderProgramID )
{
	// Do we have this shader?
	if ( this->m_mapShaderProgram.find( shaderProgramID ) == this->m_mapShaderProgram.end() )
	{	// Nope.
		std::stringstream ss;
		ss << "Could not find '" << shaderProgramID << "' shader program.";
		this->m_LastError = ss.str();
		return false;
	}
	// Found it. Are all the contained shaders OK?
	CShaderProgramDescription shaderProg = this->m_mapShaderProgram[ shaderProgramID ];
	bool bProgIsOK = true;	// Assume true...
	if ( shaderProg.vShader.source != "" )
	{
		if ( !shaderProg.vShader.bIsOK )	{	bProgIsOK = false;	}
	}
	if ( shaderProg.gShader.source != "" )
	{
		if ( !shaderProg.gShader.bIsOK )	{	bProgIsOK = false;	}
	}
	if ( shaderProg.fShader.source != "" )
	{
		if ( !shaderProg.fShader.bIsOK )	{	bProgIsOK = false;	}
	}
	// Did program link?
	if ( ( ! shaderProg.bIsOK ) || ( ! bProgIsOK ) )
	{ 
		shaderProg.bIsOK = false;
	}

	return shaderProg.bIsOK;
}

bool CGLShaderManager::IsShaderOK( std::string shaderName )
{
	// Is it there? 
	if ( this->m_mapShaderName_to_ID.find( shaderName ) == this->m_mapShaderName_to_ID.end() )
	{
		std::stringstream ss;
		ss << "Could not find '" << shaderName << "' shader program.";
		this->m_LastError = ss.str();
		return false;
	}
	// Found it... 
	GLint shaderID = this->m_mapShaderName_to_ID[ shaderName ];
	
	return this->IsShaderOK( shaderID );
}


// Uses glGetActiveAttrib and glGetActiveUniform to determine the active variables in use in the shader
// BIG FAT NOTE: This is only the "active" ones - many times, they are profiled out by the compiler
bool CGLShaderManager::LoadActiveShaderVariables( std::string shaderName )
{
	if ( this->m_mapShaderName_to_ID.find( shaderName ) == this->m_mapShaderName_to_ID.end() )
	{
		std::stringstream ss;
		ss << "Could not find '" << shaderName << "' shader program.";
		this->m_LastError = ss.str();
		return false;
	}
	// Found that name, so load the ID...
	GLuint shaderID = this->m_mapShaderName_to_ID[ shaderName ];
	//
	return this->LoadActiveShaderVariables( shaderID ) ;
}

// Called after link is OK...
// Uses glGetActiveAttrib and glGetActiveUniform to determine the active variables in use in the shader
// BIG FAT NOTE: This is only the "active" ones - many times, they are profiled out by the compiler
//bool CGLShaderManager::m_LoadShaderVariableDescriptions( CShaderProgramDescription &shaderProgDescription )
bool CGLShaderManager::LoadActiveShaderVariables( GLuint shaderID )
{
	// See if the ID is valid...
	if ( this->m_mapShaderProgram.find( shaderID ) == this->m_mapShaderProgram.end() )
	{	// Nope.
		std::stringstream ss;
		ss << "Could not find '" << shaderID << "' shader program.";
		this->m_LastError = ss.str();
		return false;
	}

	// At this point, the shader ID is valid...
	//      _   _       _ _         _                      _      _    _        
	// __ _| |_| |_ _ _(_) |__ _  _| |_ ___  __ ____ _ _ _(_)__ _| |__| |___ ___
	/// _` |  _|  _| '_| | '_ \ || |  _/ -_) \ V / _` | '_| / _` | '_ \ / -_|_-<
	//\__,_|\__|\__|_| |_|_.__/\_,_|\__\___|  \_/\__,_|_| |_\__,_|_.__/_\___/__/
	//
	// Get number of active (i.e. linked and in use) attribute variables...
	GLint numActiveVars = -1;
	glGetProgramiv( shaderID, GL_ACTIVE_ATTRIBUTES, &numActiveVars );
	if ( numActiveVars > 0 )
	{	// Load the variables....
		GLchar	name[ CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE ];	// buffer to store attribute name
		GLsizei nameSizeReturned = 0;
		for ( GLuint index = 0; index != static_cast<GLuint>( numActiveVars); index++ )
		{
			CShaderAttributeDescription theAttrib;
			theAttrib.programID = shaderID;
			theAttrib.programName = this->m_mapShaderProgram[ shaderID ].name;
			glGetActiveAttrib( shaderID,	// Shader program ID
							   index,						// attribute variable index
							   CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE,						// buffer size
							   &nameSizeReturned,			// actual size of name in buffer (returned)
							   &(theAttrib.size),			// size of attribute
							   &(theAttrib.type),			// type (GLenum)			   
							   name );						// name (in character array)
			// Convert...
			theAttrib.name.append( name );
			// Decode the type...
			theAttrib.typeAsString = this->DecodeTypeNameFromGLenum( theAttrib.type ); 
			theAttrib.index = index;
			// Update the map of shaders with this new information
			this->m_mapShaderProgram[ shaderID ].mapAttributesByName[ theAttrib.name ] = theAttrib;
		}
	}


	//                        _             _  __                 _    _         _       
	// _ _  __ _ _ __  ___ __| |  _  _ _ _ (_)/ _|___ _ _ _ __   | |__| |___  __| |__ ___
	//| ' \/ _` | '  \/ -_) _` | | || | ' \| |  _/ _ \ '_| '  \  | '_ \ / _ \/ _| / /(_-<
	//|_||_\__,_|_|_|_\___\__,_|  \_,_|_||_|_|_| \___/_| |_|_|_| |_.__/_\___/\__|_\_\/__/
	//                                                                                   
	// **********************************************************
	// https://www.opengl.org/registry/specs/ARB/uniform_buffer_object.txt
	// **********************************************************
	// Now many uniform blocks are there? 
	GLint numberOfUniformBlocks = -1;
	glGetProgramiv( shaderID, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfUniformBlocks );

	// Reset anything that was in the shader program (may have been re-linked)
	CShaderProgramDescription& currentShader = this->m_mapShaderProgram[ shaderID ];

	currentShader.vecNamedUniformBlocks.clear();
	currentShader.vecNamedUniformBlocks.reserve( numberOfUniformBlocks );
	for ( int count = 0; count != numberOfUniformBlocks; count++ ) { currentShader.vecNamedUniformBlocks.push_back( CShaderUniformBlockDescription() ); }

	// Get the information about the uniform blocks
	GLchar	name[ CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE ] = {0};	// buffer to store attribute name
	GLsizei nameSizeReturned = 0;
	for ( int blockIndex = 0; blockIndex != numberOfUniformBlocks; blockIndex++ )
	{
		// https://www.opengl.org/wiki/GLAPI/glGetActiveUniformBlock
		glGetActiveUniformBlockName( shaderID, 
		                             blockIndex, 
									 CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE, 
									 &nameSizeReturned,
									 name );

		// Get a reference to the uniform block we're playing with...
		CShaderUniformBlockDescription &CurrentUniformBlock = currentShader.vecNamedUniformBlocks[blockIndex];

		CurrentUniformBlock.nameLength = nameSizeReturned;
		CurrentUniformBlock.name = name;

		CurrentUniformBlock.programID = shaderID;
		CurrentUniformBlock.programName = this->m_mapShaderProgram[ shaderID ].name;

		// The index of the uniform buffer binding point last selected by the uniform block pecified by uniformBlockIndex? 
		//	for program? is returned. If no uniform block has been previously specified, zero is returned.
		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_BINDING, &(CurrentUniformBlock.bindingPoint) );

		// The implementation-dependent minimum total buffer object size, in basic machine units, required to hold all active uniforms in 
		//  the uniform block identified by uniformBlockIndex? is returned.
		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &(CurrentUniformBlock.dataSize) );

		// The number of active uniforms in the uniform block identified by uniformBlockIndex? is returned.
		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &(CurrentUniformBlock.numberOfActiveUniforms) );

		// Get the active uniform indices... fills an array
		GLint* activeUniformIndices = new GLint[ CurrentUniformBlock.numberOfActiveUniforms ];
		memset( activeUniformIndices, 0, CurrentUniformBlock.numberOfActiveUniforms );	// Clear it, yo
		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, activeUniformIndices );
		// Save them into the vector.
		CurrentUniformBlock.vecUniformVarIndices.clear();
		CurrentUniformBlock.vecUniformVarIndices.reserve( CurrentUniformBlock.numberOfActiveUniforms );
		for ( int count = 0; count != CurrentUniformBlock.numberOfActiveUniforms; count++ )
		{
			CurrentUniformBlock.vecUniformVarIndices.push_back( activeUniformIndices[count] );
		}

		delete [] activeUniformIndices; // Bye bye, Mr. Array

		CurrentUniformBlock.bReferencedFromVertexShader = false;
		CurrentUniformBlock.bReferencedFromTessControlShader = false;
		CurrentUniformBlock.bReferencedFromTessControlShader = false;
		CurrentUniformBlock.bReferencedFromGeometryShader = false;
		CurrentUniformBlock.bReferencedFromFragmentShader = false;
		CurrentUniformBlock.bReferencedFromComputeShader = false;

		GLint tempBool = GL_FALSE;

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromVertexShader = true; } 

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromTessControlShader = true; } 

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromTessControlShader = true; }

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromGeometryShader = true; }

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromFragmentShader = true; }

		glGetActiveUniformBlockiv( shaderID, blockIndex, GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER, &tempBool );
		if ( tempBool == GL_TRUE ) { CurrentUniformBlock.bReferencedFromComputeShader = true; }

		// Update the "uniform name to ID" map (since the vector is already indexed by ID (from zero)
		currentShader.mapNamedUniformBlockName_to_Index[ CurrentUniformBlock.name ] = blockIndex;
	}// for ( int blockIndex = 0; blockIndex != numberOfUniformBlocks; blockIndex++ )



	//	         _  __                              _      _    _        
	// _  _ _ _ (_)/ _|___ _ _ _ __   __ ____ _ _ _(_)__ _| |__| |___ ___
	//| || | ' \| |  _/ _ \ '_| '  \  \ V / _` | '_| / _` | '_ \ / -_|_-<
	// \_,_|_||_|_|_| \___/_| |_|_|_|  \_/\__,_|_| |_\__,_|_.__/_\___/__/
	//                                                                   
	// Now that we have the named unifom blocks, get additional information for the uniforms.
	// Also tie the uniform block information back into the uniforms (like the names, etc.)
	//
	// Get number of active (i.e. linked and in use) attribute variables...
	GLint numberOfUniformVars = -1;
	glGetProgramiv( shaderID, GL_ACTIVE_UNIFORMS, &numberOfUniformVars );

	if ( numberOfUniformVars > 0 )
	{	// Load the variables....
		// Erase any previous uniforms
		currentShader.vecUniformVariables.clear();
		// Make sure there's space
		currentShader.vecUniformVariables.reserve( numberOfUniformVars );
		for ( int count = 0; count != numberOfUniformVars; count++ ) { currentShader.vecUniformVariables.push_back( CShaderUniformDescription() ); }

		GLchar	name[ CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE ] = {0};	// buffer to store attribute name
		GLsizei nameSizeReturned = 0;
		for ( GLuint index = 0; index != static_cast<GLuint>( numberOfUniformVars); index++ )
		{
			CShaderUniformDescription theUniform;
			theUniform.programID = shaderID;
			theUniform.programName = this->m_mapShaderProgram[ shaderID ].name;
			glGetActiveUniform( shaderID,	// Shader program ID
							    index,						// attribute variable index
							    CGLShaderManager::MAX_SHADER_CHAR_BUFFER_SIZE,						// buffer size
							    &nameSizeReturned,			// actual size of name in buffer (returned)
							    &(theUniform.size),			// size of attribute
							    &(theUniform.type),			// type (GLenum)			   
							    name );						// name (in character array)
			// Convert...
			theUniform.name.append( name );
			// Decode the type...
			theUniform.typeAsString = this->DecodeTypeNameFromGLenum( theUniform.type ); 
			theUniform.index = index;

			// Note that the "location" is different from the "index" 
			// (Setting the "index" value here screws things up with there are named uniforms (or something like that)
			theUniform.location = glGetUniformLocation( shaderID, theUniform.name.c_str() );

			// Get the additional information for the uniforms...
			GLuint* uniformsIndex = new GLuint[1];	memset( uniformsIndex, 0, sizeof(GLuint) * 1 );	// Yes, an array of 1 item. 
			GLint* dataOut = new GLint[1];	memset( dataOut, 0, sizeof(GLint) * 1 );	// Yes, another single element array

			uniformsIndex[0] = index;		// The uniform we are interested in

			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_NAME_LENGTH, dataOut );	theUniform.nameLength = dataOut[0];

			// The uniform block index for this uniform. 
			theUniform.bIsInDefaultBlock = false;
			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_BLOCK_INDEX, dataOut );	theUniform.namedBlockIndex = dataOut[0];
			// If this uniform is not in a block, the value will be -1.
			if ( theUniform.namedBlockIndex == -1 )	{ theUniform.bIsInDefaultBlock = true; }

			// The byte offset into the beginning of the uniform block for this uniform. If the uniform is not in a block, the value will be -1.
			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_OFFSET, dataOut );		
			theUniform.blockOffsetInBytes = dataOut[0];
			theUniform.blockOffsetInFloats = theUniform.blockOffsetInBytes / sizeof( float );

			// The byte stride for elements of the array, for uniforms in a uniform block. 
			theUniform.bIsAnArray = true;
			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_ARRAY_STRIDE, dataOut );	theUniform.arrayStride = dataOut[0];
			// For non-array uniforms in a block, this value is 0. 
			// For uniforms not in a block, the value will be -1.
			if ( theUniform.arrayStride == 0 ) { theUniform.bIsAnArray = false; }

			// The byte stride for columns of a column-major matrix or rows for a row-major matrix, for uniforms in a uniform block. 
			theUniform.bIsMatrix = true;
			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_MATRIX_STRIDE, dataOut );	theUniform.matrixStride = dataOut[0];
			// For non-matrix uniforms in a block, this value is 0. 
			// For uniforms not in a block, the value will be -1.
			if ( theUniform.matrixStride == 0 ) { theUniform.bIsMatrix = false; }

			// GL_TRUE? if the matrix is row-major and the uniform is in a block. 
			// GL_FALSE? is returned if the uniform is: 
			//	- column-major, 
			//  - the uniform is not in a block (all non-block matrices are column-major), 
			//  - or simply not a matrix type.
			glGetActiveUniformsiv( shaderID, 1, uniformsIndex, GL_UNIFORM_IS_ROW_MAJOR, dataOut );	
			theUniform.bIsRowMajor = false;
			if ( dataOut[0] == GL_TRUE ) { theUniform.bIsRowMajor = true; }


			delete [] uniformsIndex;
			delete [] dataOut;

			// If the uniform isn't in the default block, then get the name of the block it's in (from the index)
			if ( ! theUniform.bIsInDefaultBlock )
			{
				theUniform.blockName = currentShader.vecNamedUniformBlocks[ theUniform.namedBlockIndex ].name;
			}

			// Update the map of shaders with this new information
			currentShader.vecUniformVariables[index] = theUniform;

		}// for ( GLuint index = 0; index != static_cast<GLuint>( numberOfUniformVars); index++ )
	}// if ( numberOfUniformVars > 0 )

	// At this point, all the uniforms are loaded, so place them in the "map uniforms by name" in the shader
	unsigned int numberOfUniforms = static_cast<unsigned int>( currentShader.vecUniformVariables.size() );
	for ( unsigned int index = 0; index != numberOfUniforms; index++ )
	{
		// Store name to location...
		currentShader.mapUniformVarName_to_Location[ currentShader.vecUniformVariables[index].name ] 
				= currentShader.vecUniformVariables[index].location;	
		// Store name to index...
		currentShader.mapUniformName_to_Index[ currentShader.vecUniformVariables[index].name ] = index;
	}



	// At this point, all the uniforms are loaded, as well as the named uniform blocks. 
	// Any uniforms in named blocks have the index, name, stride, and offset set.
	return true;
}

bool CGLShaderManager::InitBuffersFromUniformBlocks( const std::string &shaderProgramName )
{
	GLuint shaderID = this->GetShaderIDFromName( shaderProgramName );
	// Valid (didn't return zero)?
	if ( shaderID == 0 )
	{
		std::stringstream ss;
		ss << "Error: Shader program >" << shaderProgramName << "< not found.";
		this->m_LastError = ss.str();
		return false;
	}
	return this->InitBuffersFromUniformBlocks( shaderID );
}

bool CGLShaderManager::InitBuffersFromUniformBlocks( GLuint shaderProgramID )
{
	if ( !this->DoesShaderExist( shaderProgramID ) )
	{
		std::stringstream ss;
		ss << "Error: Shader program >" << shaderProgramID << "< not found.";
		this->m_LastError = ss.str();
		return false;
	}
	// Go through all the uniform blocks an init them...
	CShaderProgramDescription& currentShader = this->m_mapShaderProgram[ shaderProgramID ];


	bool bAnyErrors = false;
	std::stringstream ss;
	for( std::vector< CShaderUniformBlockDescription >::iterator itNamedBlock = currentShader.vecNamedUniformBlocks.begin();
		 itNamedBlock != currentShader.vecNamedUniformBlocks.end(); itNamedBlock++ )
	{
		if ( !this->InitBufferFromNamedUniformBlock( shaderProgramID, itNamedBlock->name ) )
		{
			ss << this->GetLastError() << std::endl;
			bAnyErrors = true;
		}
	}
	// If anything went wrong, return false (even if some of the blocks worked)
	return !bAnyErrors;
}

bool CGLShaderManager::InitBufferFromNamedUniformBlock( const std::string &shaderProgramName, const std::string &uniformBlockName )
{
	GLuint shaderID = this->GetShaderIDFromName( shaderProgramName );
	// Valid (didn't return zero)?
	if ( shaderID == 0 )
	{
		std::stringstream ss;
		ss << "Error: Shader program >" << shaderProgramName << "< not found.";
		this->m_LastError = ss.str();
		return false;
	}
	return this->InitBufferFromNamedUniformBlock( shaderID, uniformBlockName );
}

bool CGLShaderManager::InitBufferFromNamedUniformBlock( GLuint shaderProgramID, const std::string &uniformBlockName )
{
	GLenum errorEnum = GL_NO_ERROR;
	std::string errorString;
	std::string errorDetails;

	GLuint bindingPoint = 0;

	// TODO: make sure shader program is valid
	if ( ! this->DoesShaderExist( shaderProgramID ) )
	{
		std::stringstream ss;
		ss << "Error: Shader >" << shaderProgramID << "< doesn't exist.";
		this->m_LastError = ss.str();
		return false;
	}
	// At this point, it's a valid shader

	CShaderProgramDescription& currentShader = this->m_mapShaderProgram[ shaderProgramID ];
	// TODO: make sure uniformBlockName is valid
	std::map< std::string /*block name*/, GLint /*blockID*/ >::iterator itNamedBlock = currentShader.mapNamedUniformBlockName_to_Index.find(uniformBlockName);
	if ( itNamedBlock == currentShader.mapNamedUniformBlockName_to_Index.end() )
	{
		std::stringstream ss;
		ss << "Error: Shader >" << shaderProgramID << "< doesn't have a named uniform block called >" << uniformBlockName << "<";
		this->m_LastError = ss.str();
		return false;
	}


	// Do we already have this buffer? 
	std::map< std::string /*name*/, GLuint /*bindingPoint*/ >::iterator itBuffer 
								=  this->m_mapBufferNameToBindingPoint.find( uniformBlockName );
	if ( itBuffer != this->m_mapBufferNameToBindingPoint.end() )
	{	// Yup, there's already a buffer with that name
		bindingPoint =  this->m_mapBufferNameToBindingPoint[uniformBlockName];


		// Bind the uniform block to this binding point...
		GLint uniformBlockID = currentShader.mapNamedUniformBlockName_to_Index[uniformBlockName];
		// Is the current block the same size as the current buffer? 
		if ( this->m_vecUniformBufferBindings[bindingPoint].bufferSizeBytes != currentShader.vecNamedUniformBlocks[uniformBlockID].dataSize )
		{	// Nope. 
			std::stringstream ss;
			ss << "Error: Shader >" << this->m_mapShaderProgram[ shaderProgramID ].name << "<"
				<< " is trying to use an existing uniform buffer but the size is different. "
				<< "Existing block is " << this->m_vecUniformBufferBindings[bindingPoint].bufferSizeBytes << " bytes "
				<< "but the shader wants " << currentShader.vecNamedUniformBlocks[uniformBlockID].dataSize
				<< " bytes.";
			this->m_LastError = ss.str();
			return false;
		}
		// Bind this buffer to the shaders uniform block
		glUniformBlockBinding( shaderProgramID, uniformBlockID, bindingPoint );
		if ( COpenGLError::bWasThereAnOpenGLError(errorEnum) ) 
		{
			return false;
		}	



		// We're good to go.
		return true;
	}

	// If we are here, we need to create a new buffer

	// No, so we have to make one. 
	// Search for first free binding point
	for ( bindingPoint = 0; bindingPoint != this->m_maxUniformBindings; bindingPoint++ )
	{
		if ( ! this->m_vecUniformBufferBindings[bindingPoint].bInUse )
		{	// This is the one
			break;	// Exit for loop
		}
	}
	// Find a point
	if ( bindingPoint == this->m_maxUniformBindings )
	{	// No, we didn't find a free one (Holy crap! How could this happen?)
		std::stringstream ss;
		ss << "Error: Can't find a free uniform block binding point. "
			<< "GL_MAX_UNIFORM_BUFFER_BINDINGS = " << this->m_maxUniformBindings;
		this->m_LastError = ss.str();
		return false;
	}

	// At this point, we have a free binding point (bindingPoint) but no buffer, yet
	glGenBuffers( 1, &( this->m_vecUniformBufferBindings[bindingPoint].bufferID ) );
	glBindBuffer( GL_UNIFORM_BUFFER, this->m_vecUniformBufferBindings[bindingPoint].bufferID );
	if ( COpenGLError::bWasThereAnOpenGLError(errorEnum) ) 
	{
		return false;
	}

	// Get sizer of buffer needed
	GLint uniformBlockID = currentShader.mapNamedUniformBlockName_to_Index[uniformBlockName];
	GLuint sizeOfBuffer = currentShader.vecNamedUniformBlocks[ uniformBlockID ].dataSize;

	// Size returned is in "system units" aka "bytes" but we are storing in floats...
	// ...since we are allocating bytes, we need to see how many floats that is.
	// Why is this? Well, malloc, which is "C" requires bytes. But we're using new, which is "C++"
	unsigned int numberOfFloats = sizeOfBuffer/sizeof(float);
	this->m_vecUniformBufferBindings[bindingPoint].pBufferData = new float[ numberOfFloats ];
	//this->m_vecUniformBufferBindings[bindingPoint].pBufferData = new char[ sizeOfBuffer ];

	glBufferData( GL_UNIFORM_BUFFER, sizeOfBuffer, this->m_vecUniformBufferBindings[bindingPoint].pBufferData, GL_DYNAMIC_DRAW );
	if ( COpenGLError::bWasThereAnOpenGLError(errorEnum) ) 
	{
		return false;
	}	
	glBindBufferBase( GL_UNIFORM_BUFFER, bindingPoint, this->m_vecUniformBufferBindings[bindingPoint].bufferID );
	if ( COpenGLError::bWasThereAnOpenGLError(errorEnum) ) 
	{
		return false;
	}	

	// And now connect this to the shaders uniform block
	glUniformBlockBinding( shaderProgramID, uniformBlockID, bindingPoint );
	if ( COpenGLError::bWasThereAnOpenGLError(errorEnum) ) 
	{
		return false;
	}	
	// We're good to go.

	// Update the name of the uniform block in the binding block (it's the ID that's the 
	//  critical value, but this is helpful for debugging)
	this->m_vecUniformBufferBindings[bindingPoint].uniformBlockName = uniformBlockName;
	this->m_vecUniformBufferBindings[bindingPoint].bufferSizeBytes = sizeOfBuffer;
	this->m_vecUniformBufferBindings[bindingPoint].bufferSizeFloats = numberOfFloats;
	this->m_vecUniformBufferBindings[bindingPoint].bindingPoint = bindingPoint;
	this->m_vecUniformBufferBindings[bindingPoint].bInUse = true;

	// Update the binding point in the named uniform structure (in case we want to print it out or something)
	currentShader.vecNamedUniformBlocks[uniformBlockID].bindingPoint = bindingPoint;



	return true;
}

	// Load the information from the Uniform Variables from the buffer info.

	//// Test setting stuff
	//unsigned int uniform_bring_index = currentShader.mapUniformVarName_to_Index["SexyBlock.Bring"];
	//unsigned int offset_bring = currentShader.vecUniformVariables[uniform_bring_index].blockOffsetInFloats;

	//unsigned int uniform_it_index = currentShader.mapUniformVarName_to_Index["SexyBlock.it"];
	//unsigned int offset_it = currentShader.vecUniformVariables[uniform_it_index].blockOffsetInFloats;

	//unsigned int uniform_back_index =  currentShader.mapUniformVarName_to_Index["SexyBlock.back"];
	//unsigned int offset_back = currentShader.vecUniformVariables[uniform_back_index].blockOffsetInFloats;

	//*(this->m_vecUniformBufferBindings[bindingPoint].pBufferData + offset_bring)	= 0.0f;		// Red
	//*(this->m_vecUniformBufferBindings[bindingPoint].pBufferData + offset_it)		= 0.0f;		// Green
	//*(this->m_vecUniformBufferBindings[bindingPoint].pBufferData + offset_back)		= 0.5f;		// Blue

	//// copy this up
	//glBufferSubData( GL_UNIFORM_BUFFER, 0 /*offset*/, 
	//	             sizeOfBuffer /*number of bytes*/, 
	//				 this->m_vecUniformBufferBindings[bindingPoint].pBufferData );
	//if ( COpenGLError::bWasThereAnOpenGLError( errorEnum, errorString, errorDetails ) )
	//{
	//	return false;
	//}


bool CGLShaderManager::UpdateNamedBlockFromBuffer( const std::string &shaderProgramName, const std::string &uniformBlockName )
{
	GLuint shaderID = this->GetShaderIDFromName( shaderProgramName );
	// Valid (didn't return zero)?
	if ( shaderID == 0 )
	{
		std::stringstream ss;
		ss << "Error: Shader program >" << shaderProgramName << "< not found.";
		this->m_LastError = ss.str();
		return false;
	}
	return this->UpdateNamedBlockFromBuffer( shaderID, uniformBlockName );
}

bool CGLShaderManager::UpdateNamedBlockFromBuffer( GLuint shaderProgramID, const std::string &uniformBlockName )
{
	CShaderProgramDescription currentShader;
	currentShader.ID = shaderProgramID;

	// ShaderID is valid?
	if ( ! this->DoesShaderExist( shaderProgramID ) )
	{
		std::stringstream ss;
		ss << "Error: Shader >" << shaderProgramID << "< doesn't exist.";
		this->m_LastError = ss.str();
		return false;
	}

	// We can assume shader exists because of DoesShaderExist() call
	// (if not, then this is going to end in tears, aka crash the program)
	this->GetShaderProgramInfo( currentShader );

	CShaderUniformBlockDescription currentBlock;
	if ( !currentShader.GetNamedBlock( uniformBlockName, currentBlock ) )
	{
		std::stringstream ss;
		ss << "Error: Uniform block >" << uniformBlockName << "< for shader ID >" << shaderProgramID 
			<< "< (which is shader >" << currentShader.name << "< ) doesn't exist or isn't valid (did you Init the block?).";
		this->m_LastError = ss.str();
		return false;
	}

	// At this point, we have a valid uniform block.
	// Get the binding point for this uniform block and check if it's valid...
	if ( !this->m_vecUniformBufferBindings[ currentBlock.bindingPoint ].bInUse )
	{
		std::stringstream ss;
		ss << "Error: Binding point in named uniform block >" << uniformBlockName << "< for shader ID >" << shaderProgramID
			<< "< (which is shader >" << currentShader.name << "< ) is pointing to a binding point >" 
			<< currentBlock.bindingPoint << "< which isn't in use (did you Init the block?).";
		this->m_LastError = ss.str();
		return false;
	}

	// Binding point is valid and in use
	CUniformBlockBuffer &currentBuffer = this->m_vecUniformBufferBindings[ currentBlock.bindingPoint ];	// So it's easier to read
	
	// copy this up
	glBufferSubData( GL_UNIFORM_BUFFER, 0 /*offset*/, 
		             currentBuffer.bufferSizeBytes /*number of bytes*/, 
					 currentBuffer.pBufferData );
	std::string errorString, errorDetails;
	GLenum errorEnum = NO_ERROR;
	if ( COpenGLError::bWasThereAnOpenGLError( errorEnum, errorString, errorDetails ) )
	{
		std::stringstream ss;
		ss << "Error copying buffer to named uniform block >" << uniformBlockName << "<" << std::endl
			<< "Error copying buffer to named uniform block. GL error string: " << errorString << std::endl
			<< "GL error details: " << errorDetails;
		this->m_LastError = ss.str();
		return false;
	}
	return true;
}

bool GetUniformVarDesc( const std::string &name, CShaderUniformDescription &UniformVarDesc );

GLuint CGLShaderManager::GetMaxUniformBindings(void)
{
	GLint maxUniformBindings = 0;
	glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBindings );
	return maxUniformBindings;
}


GLint CGLShaderManager::GetShaderIDFromName( std::string shaderName )
{
	if ( this->m_mapShaderName_to_ID.find( shaderName ) == this->m_mapShaderName_to_ID.end() )
	{
		return 0;		// 0 in OpenGL usually means "bad" or an error
	}
	// Found that name, so load the ID...
	return this->m_mapShaderName_to_ID[ shaderName ];
}


bool CGLShaderManager::DoesShaderExist( GLuint shaderProgramID )
{
	std::map< GLuint, CShaderProgramDescription >::iterator itShader = this->m_mapShaderProgram.find( shaderProgramID );
	if ( itShader == this->m_mapShaderProgram.end() )
	{	// Nope, doesn't exist
		return false;
	}
	return true;
}

bool CGLShaderManager::DoesShaderExist( std::string shaderName )
{
	if ( GetShaderIDFromName( shaderName ) == 0 )
	{	// Nope, doesn't exist
		return false;
	}
	return true;
}

bool CGLShaderManager::GetShaderProgramInfo( CShaderProgramDescription &shaderProgDescription )
{
	// If shaderProgramID != 0, look for that ID...
	if ( shaderProgDescription.ID == 0 )
	{	// See if there is a match to the program name...
		if ( this->m_mapShaderName_to_ID.find( shaderProgDescription.name ) == this->m_mapShaderName_to_ID.end() )
		{
			std::stringstream ss;
			ss << "Could not find '" << shaderProgDescription.name << "' shader program.";
			this->m_LastError = ss.str();
			return false;
		}
		// Found that name, so load the ID...
		shaderProgDescription.ID = this->m_mapShaderName_to_ID[ shaderProgDescription.name ];
	}
	else
	{	// See if we have that ID...
		if ( this->m_mapShaderProgram.find( shaderProgDescription.ID ) == this->m_mapShaderProgram.end() )
		{	// Nope.
			std::stringstream ss;
			ss << "Could not find '" << shaderProgDescription.ID << "' shader program.";
			this->m_LastError = ss.str();
			return false;
		}
		// We have that ID, so load the shader program name
		shaderProgDescription.name = this->m_mapShaderProgram[ shaderProgDescription.ID ].name;
	}
	// At this point, the shader ID (and program name) are both valid...
	shaderProgDescription = this->m_mapShaderProgram[ shaderProgDescription.ID ];

	return true;
}

// Destroys everything (called when program exits or you are deleting all shaders)
bool CGLShaderManager::ShutDown(void)
{
	for ( std::map< GLuint, CShaderProgramDescription >::iterator itProg = this->m_mapShaderProgram.begin();
		  itProg != this->m_mapShaderProgram.end(); ++itProg )
	{
		// If the shader ID are zero then they aren't valid (or have already been deleted)
		if ( itProg->second.vShader.ID != 0 )
		{
			glDetachShader( itProg->second.ID, itProg->second.vShader.ID );
			glDeleteShader( itProg->second.vShader.ID );
			itProg->second.vShader.ID = 0;	// Indicating shader is invalid or deleted
			itProg->second.vShader.type = GLSHADERTYPES::UNKNOWN;
		}
		if ( itProg->second.fShader.ID != 0 )
		{
			glDetachShader( itProg->second.ID, itProg->second.fShader.ID );
			glDeleteShader( itProg->second.fShader.ID );
			itProg->second.fShader.ID = 0;
			itProg->second.fShader.type = GLSHADERTYPES::UNKNOWN;
		}
		if ( itProg->second.gShader.ID != 0 )
		{
			glDetachShader( itProg->second.ID, itProg->second.gShader.ID );
			glDeleteShader( itProg->second.gShader.ID );
			itProg->second.gShader.ID = 0;
			itProg->second.gShader.type = GLSHADERTYPES::UNKNOWN;
		}
		if ( itProg->second.tContShader.ID != 0 )
		{
			glDetachShader( itProg->second.ID, itProg->second.tContShader.ID );
			glDeleteShader( itProg->second.tContShader.ID );
			itProg->second.tContShader.ID = 0;
			itProg->second.tContShader.type = GLSHADERTYPES::UNKNOWN;
		}
		if ( itProg->second.tEvalShader.ID != 0 )
		{
			glDetachShader( itProg->second.ID, itProg->second.tEvalShader.ID );
			glDeleteShader( itProg->second.tEvalShader.ID );
			itProg->second.tEvalShader.ID = 0;
			itProg->second.tEvalShader.type = GLSHADERTYPES::UNKNOWN;
		}
		// All the shaders are gone, so delete the program
		glDeleteProgram( itProg->second.ID );
	}
	// All done
	return true;
}


std::string CGLShaderManager::DecodeTypeNameFromGLenum( GLenum type )
{
	switch ( type )
	{
	case GL_FLOAT:
		return "GL_FLOAT";			break;
	case GL_FLOAT_VEC2:	// 0x8B50
		return "GL_FLOAT_VEC2";		break;
	case GL_FLOAT_VEC3:	// 0x8B51
		return "GL_FLOAT_VEC3";		break;
	case GL_FLOAT_VEC4:	// 0x8B52
		return "GL_FLOAT_VEC4";		break;
	case GL_INT_VEC2:	// 0x8B53
		return "GL_INT_VEC2";		break;
	case GL_INT_VEC3:	// 0x8B54
		return "GL_INT_VEC3";		break;
	case GL_INT_VEC4:	// 0x8B55
		return "GL_INT_VEC4";		break;	
	case GL_FLOAT_MAT2:
		return "GL_FLOAT_MAT2";		break;
	case GL_FLOAT_MAT3:
		return "GL_FLOAT_MAT3";		break;
	case GL_FLOAT_MAT4:
		return "GL_FLOAT_MAT4";		break;
	case GL_FLOAT_MAT2x3:	// OpenGL 2.1 only
		return "GL_FLOAT_MAT2x3";	break;
	case GL_FLOAT_MAT2x4:	// OpenGL 2.1 only
		return "GL_FLOAT_MAT2x4";	break;
	case GL_FLOAT_MAT3x2:	// OpenGL 2.1 only 
		return "GL_FLOAT_MAT3x2";	break;
	case GL_FLOAT_MAT3x4:	// OpenGL 2.1 only 
		return "GL_FLOAT_MAT3x4";	break;
	case GL_FLOAT_MAT4x2:	// OpenGL 2.1 only
		return "GL_FLOAT_MAT4x2";	break;
	case GL_FLOAT_MAT4x3:	// OpenGL 2.1 only
		return "GL_FLOAT_MAT4x3";	break;

	case GL_BOOL:	// 0x8B56
		return "GL_BOOL";			break;
	case GL_BOOL_VEC2: // 0x8B57
		return "GL_BOOL_VEC2";		break;
	case GL_BOOL_VEC3: // 0x8B58
		return "GL_BOOL_VEC3";		break;
	case GL_BOOL_VEC4: // 0x8B59
		return "GL_BOOL_VEC4";		break;
	case GL_SAMPLER_1D: // 0x8B5D
		return "GL_SAMPLER_1D";		break;
	case GL_SAMPLER_2D: // 0x8B5E
		return "GL_SAMPLER_2D";		break;
	case GL_SAMPLER_3D: // 0x8B5F
		return "GL_SAMPLER_3D";		break;
	case GL_SAMPLER_CUBE: // 0x8B60
		return "GL_SAMPLER_CUBE";		break;
	case GL_SAMPLER_1D_SHADOW: // 0x8B61
		return "GL_SAMPLER_1D_SHADOW";		break;
	case GL_SAMPLER_2D_SHADOW: // 0x8B62
		return "GL_SAMPLER_2D_SHADOW";		break;

	}
	return "GL_ID_UNKNOWN";
}

