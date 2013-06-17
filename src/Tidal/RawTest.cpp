#include <Tidal/Tidal.h>
#include <fstream>
#include <Frames/renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float ang = 0.0f;


struct Vertex
{
	GLfloat pos[3];
	GLfloat texcoord[2];
	GLfloat normal[3];
};


std::vector<Vertex> vertices;
std::vector<GLint> indices;

GLuint vertId;
GLuint indexId;

void RawTest::RenderElement(Frames::Renderer *renderer) const
{
	Frame::RenderElement(renderer);

	glClear(GL_DEPTH_BUFFER_BIT);

	int vpDimensions[4];
	glGetIntegerv(GL_VIEWPORT, vpDimensions);

	int vpLeft = static_cast<int>(GetLeft());
	int vpTop = static_cast<int>(GetTop());
	int vpWidth = static_cast<int>(GetWidth());
	int vpHeight =  static_cast<int>(GetHeight());
	int vpBottom = vpTop + vpHeight;
	int marginBottom = vpDimensions[3] - vpBottom;

	glViewport(vpLeft, marginBottom, vpWidth, vpHeight);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// use our shader, and see if anything breaks in Frames (it did, need to clear the program at the end)
	glUseProgram(ProgramID);	

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, (float)vpWidth / (float)vpHeight, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
		glm::vec3(4,3,6), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
	Model = glm::rotate(Model, ang, glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::rotate(Model, ang / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	ang += 0.1f;

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	glm::mat4 MV4         = View * Model; // Remember, matrix multiplication is the other way around
	glm::mat3 MV = glm::mat3(MV4);


	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(MatrixIDMV, 1, GL_FALSE, &MV[0][0]);

	glDisable(GL_CULL_FACE);
	/*

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
 
	glDisableVertexAttribArray(0);

	*/

	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vertId);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	glUseProgram(0);	

	glDisable(GL_DEPTH_TEST);

	glViewport(vpDimensions[0], vpDimensions[1], vpDimensions[2], vpDimensions[3]);


	renderer->RebindBuffers();
}


void RawTest::Init()
{
	ang = 0.0f;

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
	string vertex_file_path = "test.vs";
	string fragment_file_path = "test.ps";

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
 
	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
 
	GLint Result = GL_FALSE;
	int InfoLogLength;
 
	// Compile Vertex Shader
	cout << "Compiling shader : " << vertex_file_path.c_str() << endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
 
	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
	// Compile Fragment Shader
	cout << "Compiling shader : " << fragment_file_path.c_str() << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
 
	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
	// Link the program
	fprintf(stdout, "Linking program\n");
	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
 
	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
 
	cout << "Program ID: " << ProgramID << endl;

	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	MatrixIDMV = glGetUniformLocation(ProgramID, "MV");


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	/*
	static const GLfloat g_vertex_buffer_data[] = 
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};
	*/

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

 
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
 
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
 
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("ship.obj", 
        aiProcess_CalcTangentSpace       | 
        // aiProcess_Triangulate            |
        // aiProcess_JoinIdenticalVertices  |
		aiProcess_FlipWindingOrder		 |
		// aiProcess_GenNormals			 |
        aiProcess_SortByPType);
  
	cout << scene->mNumMeshes << " meshes" << endl;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];
		cout << mesh->mNumVertices << " vertices, " << mesh->mNumFaces << " faces in mesh" << endl;	

		for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) 
		{
			const aiVector3D* pPos = &(mesh->mVertices[i]);
			const aiVector3D* pNormal = mesh->HasNormals() ? &(mesh->mNormals[i]) : &Zero3D;
			const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &Zero3D;

			Vertex vert;
			vert.pos[0] = pPos->x;
			vert.pos[1] = pPos->y;
			vert.pos[2] = pPos->z;
			vert.texcoord[0] = pTexCoord->x;
			vert.texcoord[1] = pTexCoord->y;
			vert.normal[0] = pNormal->x;
			vert.normal[1] = pNormal->y;
			vert.normal[2] = pNormal->z;
			vertices.push_back(vert);
		}

		for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) 
		{
			const aiFace& Face = mesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			indices.push_back(Face.mIndices[0]);
			indices.push_back(Face.mIndices[1]);
			indices.push_back(Face.mIndices[2]);
		}
	}

	glGenBuffers(1, &vertId);
	glGenBuffers(1, &indexId);

	glBindBuffer(GL_ARRAY_BUFFER, vertId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}


void RawTest::Cleanup()
{
}
