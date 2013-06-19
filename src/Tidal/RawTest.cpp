#include <Tidal/Tidal.h>
#include <fstream>
#include <Frames/renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float ang = 0.0f;



// Try to do this using VAOs...
enum VAO_IDs { vaoMesh, NumVAOs };
enum Buffer_IDs { bufMesh, bufMeshIndex, NumBuffers };
enum Attrib_IDs { vPosition = 0, vTexCoord = 1, vNormal = 2 };
enum Shader_IDs { vsTest, fsTest, NumShaders };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint Shaders[NumShaders];

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
GLuint texId;
sf::Image tex;

void RawTest::RenderElement(Frames::Renderer *renderer) const
{
	// Frame::RenderElement(renderer);

	int vpDimensions[4];
	glGetIntegerv(GL_VIEWPORT, vpDimensions);

	int vpLeft = static_cast<int>(GetLeft());
	int vpTop = static_cast<int>(GetTop());
	int vpWidth = static_cast<int>(GetWidth());
	int vpHeight =  static_cast<int>(GetHeight());
	int vpBottom = vpTop + vpHeight;
	int marginBottom = vpDimensions[3] - vpBottom;

	glViewport(vpLeft, marginBottom, vpWidth, vpHeight);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, (float)vpWidth / (float)vpHeight, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
		glm::vec3(2,1,3), // Camera is at (4,3,3), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
	Model = glm::rotate(Model, ang, glm::vec3(0.0f, 1.0f, 0.0f));
	//Model = glm::rotate(Model, ang / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	ang += 0.1f;

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
	glm::mat4 MV4         = View * Model; // Remember, matrix multiplication is the other way around
	glm::mat3 MV = glm::mat3(MV4);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBindVertexArray(VAOs[vaoMesh]);

	glUseProgram(ProgramID);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MV4[0][0]);
	glUniformMatrix4fv(MatrixIDMV, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(MatrixView, 1, GL_FALSE, &View[0][0]);

	GLint texloc = glGetUniformLocation(ProgramID, "color_texture"); // get the name of the sampler in the shader
	glUniform1i(texloc, 0); // tell the sampler to use texture unit 0

	glActiveTexture(GL_TEXTURE0); // Activate texture unit 0...
	glBindTexture(GL_TEXTURE_2D, texId); // ...and bind it to the texture we want to use
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	glViewport(vpDimensions[0], vpDimensions[1], vpDimensions[2], vpDimensions[3]);

	// renderer->RebindBuffers();
}


void RawTest::Init()
{
	ang = 0.0f;

	// BUILD THE SHADER PROGRAM

	Shaders[vsTest] = glCreateShader(GL_VERTEX_SHADER);
	Shaders[fsTest] = glCreateShader(GL_FRAGMENT_SHADER);
 
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
	glShaderSource(Shaders[vsTest], 1, &VertexSourcePointer , NULL);
	glCompileShader(Shaders[vsTest]);
 
	// Check Vertex Shader
	glGetShaderiv(Shaders[vsTest], GL_COMPILE_STATUS, &Result);
	glGetShaderiv(Shaders[vsTest], GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(Shaders[vsTest], InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
	// Compile Fragment Shader
	cout << "Compiling shader : " << fragment_file_path.c_str() << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(Shaders[fsTest], 1, &FragmentSourcePointer , NULL);
	glCompileShader(Shaders[fsTest]);
 
	// Check Fragment Shader
	glGetShaderiv(Shaders[fsTest], GL_COMPILE_STATUS, &Result);
	glGetShaderiv(Shaders[fsTest], GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(Shaders[fsTest], InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
	// Link the program
	fprintf(stdout, "Linking program\n");
	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, Shaders[vsTest]);
	glAttachShader(ProgramID, Shaders[fsTest]);
	glLinkProgram(ProgramID);
 
	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
	glDeleteShader(Shaders[vsTest]);
	glDeleteShader(Shaders[fsTest]);
 
	cout << "Program ID: " << ProgramID << endl;

	MatrixID = glGetUniformLocation(ProgramID, "modelView");
	MatrixIDMV = glGetUniformLocation(ProgramID, "projection");
	MatrixView = glGetUniformLocation(ProgramID, "view");


	// LOAD THE TEXTURE

	tex.loadFromFile("Metal.png");
    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,128,128,0,GL_RGBA,GL_UNSIGNED_BYTE,tex.getPixelsPtr());
    glBindTexture(GL_TEXTURE_2D, 0);


	// LOAD THE MESH

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("wildtide2.obj", 
        aiProcess_CalcTangentSpace       | 
        aiProcess_Triangulate            |
        // aiProcess_JoinIdenticalVertices  |
		aiProcess_FlipWindingOrder		 |
		aiProcess_GenNormals			 |
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


	// SETUP THE VAO FOR DRAWING THE MESH

	glGenVertexArrays(NumVAOs, VAOs);
	glGenBuffers(NumBuffers, Buffers);

	// Populate vaoMesh with all of the state needed to draw the mesh
	glBindVertexArray(VAOs[vaoMesh]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[bufMesh]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[bufMeshIndex]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vTexCoord);
    glEnableVertexAttribArray(vNormal);

	glBindVertexArray(0);
}


void RawTest::Cleanup()
{
}
