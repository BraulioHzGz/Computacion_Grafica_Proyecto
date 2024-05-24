/*
	Animación != Movimiento
				Solo 1 transformación geométrica -> no es animación.
	Animación básica: banderas, condicionales y ciclos
	Animación compleja: básicas anidadas, funciones y algoritmos de fenómenos físicos.
*/


//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <time.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;
Texture MAINTEXT;
Texture INTEGRANTES;
//Texture AgaveTexture;

Model escenario_Antes;
Model escenario_Durante;
Model escenario_Despues;
Model MuroBlanco;

Model aguila;
Model mapache;
Model ajolote;
Model lobo;
Model AlaDerechaAguila;
Model AlaIzquierdaAguila;
Model CuerpoAguila;
Model CuerpoAjolote;
Model ColaAjolote;
Model CuerpoAbeja;
Model AlaDerechaAbeja;
Model AlaIzquierdaAbeja;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];

SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	unsigned int MAINTEXT_Indices[] = {
		0,1,2,
		0,2,3,
	};

	GLfloat MAINTEXT_Vertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	unsigned int INTEGRANTES_Indices[] = {
		0,1,2,
		0,2,3,
	};

	GLfloat INTEGRANTES_Vertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(MAINTEXT_Vertices, MAINTEXT_Indices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(INTEGRANTES_Vertices, INTEGRANTES_Indices, 32, 6);
	meshList.push_back(obj6);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
}

void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(-0.4f, -1.5f, -8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.02f, 0.1f);
	//VELOCIDAD ESTANDAR: 0.02

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	/*AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();*/
	MAINTEXT = Texture("Textures/TIPOGRAFIAS/TXT_INICIAL.tga");
	MAINTEXT.LoadTextureA();
	INTEGRANTES = Texture("Textures/TIPOGRAFIAS/INTEGRANTES.tga");
	INTEGRANTES.LoadTextureA();

	//CARGA DE LOS ESCENARIOS:
	MuroBlanco = Model();
	MuroBlanco.LoadModel("Models/MuroBlanco.obj");
	escenario_Antes = Model();
	escenario_Antes.LoadModel("Models/escenario_antes_optimizado.obj");
	escenario_Durante = Model();
	escenario_Durante.LoadModel("Models/escenario_actual.obj");
	/*escenario_Despues = Model();
	escenario_Despues.LoadModel("Models/ciudad_despues.obj");*/

	//CARGA DE LOS ANIMALES:
	//AGUILA
	aguila = Model();
	aguila.LoadModel("Models/Aguila_Texturizada_Final.obj");
	AlaDerechaAguila = Model();
	AlaDerechaAguila.LoadModel("Models/AguilaAlaDer.obj");
	AlaIzquierdaAguila = Model();
	AlaIzquierdaAguila.LoadModel("Models/AguilaAlaIzq.obj");
	CuerpoAguila = Model();
	CuerpoAguila.LoadModel("Models/cuerpoAguila.obj");

	CuerpoAjolote = Model();
	CuerpoAjolote.LoadModel("Models/cuerpoAjolote.obj");
	ColaAjolote = Model();
	ColaAjolote.LoadModel("Models/colaAjolote.obj");

	CuerpoAbeja = Model();
	CuerpoAbeja.LoadModel("Models/CuerpoAbeja.obj");
	AlaDerechaAbeja = Model();
	AlaDerechaAbeja.LoadModel("Models/AlaDerAbeja.obj");
	AlaIzquierdaAbeja = Model();
	AlaIzquierdaAbeja.LoadModel("Models/AlaIzqAbeja.obj");

	mapache = Model();
	mapache.LoadModel("Models/Mapache_texturizado_final.obj");
	ajolote = Model();
	ajolote.LoadModel("Models/Ajolote_texturizado.obj");
	lobo = Model();
	lobo.LoadModel("Models/Lobo_texturizado.obj");

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, 1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	/*//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;*/

	unsigned int spotLightCount = 0;
	//linterna
	/*spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;*/

	//luz fija de color naranja
	//spotLights1[0] = SpotLight(1.0f, 1.0f, 1.0f,
	//	1.0f, 0.5f,
	//	0.0f, 0.0f, 0.0f,	//Posición inicial
	//	-1.0f, 0.0f, 0.0f,	//Dirección en la que apunta	(-x)
	//	1.0f, 0.1f, 0.01f,	//con	lin		exp
	//	30.0f);				//apertura
	//spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//Variables para cambio de los escenarios
	float movEscenario1 = 0.0f;
	float movEscenario2 = 0.0f;
	float movEscenario3 = 0.0f;

	//Variables para animación del águila
	float anguloAguila = 0.0f;
	float anguloAguilaOffset = 0.01;
	float anguloVistaAguila = 0.0f;
	float anguloVistaAguilaOffset = 0.58;
	float radio = 3.0f;
	float rangoAleteoAguila = 0.0f;
	float rangoAleteoOffsetAguila = 0.35f;

	//Variables para animación del ajolote
	float rangoAleteoColaAjolote = 30.0f;
	float rangoAleteoColaAjoloteOffset = 0.03f;
	float movAjolote1 = 0.0f;
	float movAjolote1Offset = 0.005;
	float rotarAjolote1 = 0.0f;
	float rotarAjolote1Offset = 1.0f;
	float movAjolote2 = 0.0f;
	float movAjolote2Offset = 0.005;
	float rotarAjolote2 = 0.0f;
	float rotarAjolote2Offset = 1.0f;
	bool cambioA1 = false;
	bool cambioA2 = false;

	//Variables para animación de la abeja
	float rangoAleteoAbeja = 0.0f;
	float rangoAleteoAbejaOffset = 10.0f;
	float movAbejaY = 0.0f;
	float movAbejaYOffset = 0.05f;
	float anguloAbeja = 0.0f;
	float anguloAbejaOffset = 0.01;
	float radioAbeja = 0.6f;

	//Variables para la animación del texto del muro blanco
	float tooffsetTU = 0.0f;
	float tooffsetTV = 0.0f;
	

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();
		//DeltaTime = estandarizar ciclos de reloj
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		if (mainWindow.GetEscenario1() || mainWindow.GetEscenario3()) {
			std::vector<std::string> skyboxFaces;
			skyboxFaces.push_back("Textures/Skybox/skyrender00010.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00040.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00060.tga");//piso
			skyboxFaces.push_back("Textures/Skybox/skyrender00030.tga");//cielo
			skyboxFaces.push_back("Textures/Skybox/skyrender00050.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00020.tga");

			skybox = Skybox(skyboxFaces);
		}
		else if (mainWindow.GetEscenario2()) {
			std::vector<std::string> skyboxFaces;
			skyboxFaces.push_back("Textures/Skybox/skyrender00011.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00041.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00061.tga");//piso
			skyboxFaces.push_back("Textures/Skybox/skyrender00031.tga");//cielo
			skyboxFaces.push_back("Textures/Skybox/skyrender00051.tga");
			skyboxFaces.push_back("Textures/Skybox/skyrender00021.tga");

			skybox = Skybox(skyboxFaces);
		}

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		glm::mat4 model(1.0);		//Matriz para todo lo demás
		glm::mat4 modelaux(1.0);	//Matriz para la jerarquía
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffsetTEXT = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffsetTEXT));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffsetTEXT));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//CONDICIONES PARA EL MOVIMIENTO DE ESCENARIOS
		if (mainWindow.GetEscenario() == 0) {
			movEscenario1 = 0.0f;
			movEscenario2 = 15.0f;
			movEscenario3 = 30.0f;
		}
		else if (mainWindow.GetEscenario() == 1) {
			movEscenario1 = 30.0f;
			movEscenario2 = 0.0f;
			movEscenario3 = 15.0f;
		}
		else if (mainWindow.GetEscenario() >= 2) {
			movEscenario1 = 15.0f;
			movEscenario2 = 30.0f;
			movEscenario3 = 0.0f;
		}

		//CONDICIONES Y ESTRUCTURAS PARA CONTROLAR ANIMACIONES DEPENDIENDO QUE ESCENARIO ESTÉ ACTIVO
		if (mainWindow.GetEscenario1()) {
			anguloAguila += anguloAguilaOffset * deltaTime;
			anguloVistaAguila += anguloVistaAguilaOffset * deltaTime;
			rangoAleteoAguila += rangoAleteoOffsetAguila * deltaTime;
			rangoAleteoColaAjolote += rangoAleteoColaAjoloteOffset * deltaTime;
			movAbejaY += movAbejaYOffset * deltaTime;
			anguloAbeja += anguloAbejaOffset * deltaTime;

			//Condiciones para evitar desbordamiento de las variables
			if (anguloAguila > 360.0f) { anguloAguila = 0.0f; }				
			if (rangoAleteoColaAjolote > 90.0f) { rangoAleteoColaAjolote = 0.0f; }
			if (movAbejaY > 90.0f) { movAbejaY = 0.0f; }
			if (anguloAbeja > 360.0f) { anguloAbeja = 0.0f; }

			//Condiciones para el aleteo del águila
			if (rangoAleteoAguila >= 30.0f) {
				rangoAleteoOffsetAguila = -fabs(rangoAleteoOffsetAguila); // Cambia la dirección a decreciente
			}
			else if (rangoAleteoAguila <= -25.0f) {
				rangoAleteoOffsetAguila = fabs(rangoAleteoOffsetAguila); // Cambia la dirección a creciente
			}
			rangoAleteoAguila += rangoAleteoOffsetAguila * deltaTime;

			//Condiciones para el aleteo de la abeja
			if (rangoAleteoAbeja >= 30.0f) {
				rangoAleteoAbeja = -fabs(rangoAleteoAbejaOffset);
			}
			else if (rangoAleteoAbeja <= -30.0f) {
				rangoAleteoAbeja = fabs(rangoAleteoAbejaOffset);
			}
			rangoAleteoAbeja += rangoAleteoAbejaOffset * deltaTime;

			//Condiciones para el movimiento del ajolote 1
			if (cambioA1 == false) {
				if (movAjolote1 < 4.0f) {
					movAjolote1 += movAjolote1Offset * deltaTime;
				}
				else {
					if (rotarAjolote1 < 180.0f) {
						rotarAjolote1 += rotarAjolote1Offset * deltaTime;
					}
					else {
						cambioA1 = true;
					}

				}
			}
			else {
				if (movAjolote1 > 0.0f) {
					movAjolote1 -= movAjolote1Offset * deltaTime;
				}
				else {
					if (rotarAjolote1 > 0.0f) {
						rotarAjolote1 -= rotarAjolote1Offset * deltaTime;
					}
					else {
						cambioA1 = false;
					}

				}
			}

			//Condiciones para el movimiento del ajolote 2
			if (cambioA2 == false) {
				if (movAjolote2 < 5.0f) {
					movAjolote2 += movAjolote2Offset * deltaTime;
				}
				else {
					if (rotarAjolote2 < 180.0f) {
						rotarAjolote2 += rotarAjolote2Offset * deltaTime;
					}
					else {
						cambioA2 = true;
					}

				}
			}
			else {
				if (movAjolote2 > 0.0f) {
					movAjolote2 -= movAjolote2Offset * deltaTime;
				}
				else {
					if (rotarAjolote2 > 0.0f) {
						rotarAjolote2 -= rotarAjolote2Offset * deltaTime;
					}
					else {
						cambioA2 = false;
					}

				}
			}
		}

		//AGUILA 1: ESTANQUE
		//Instancia del cuerpo del águila
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-5.5f + radio * cos(anguloAguila), 3.0f, -5.5f + radio * sin(anguloAguila)));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (-anguloVistaAguila) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAguila.RenderModel();

		//Instancia del ala derecha del águila
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.02f, 0.05f, 0.1f));
		model = glm::rotate(model, (-rangoAleteoAguila) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDerechaAguila.RenderModel();

		//Instancia del ala izquierda del águila
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.02f, 0.05f, 0.1f));
		model = glm::rotate(model, (rangoAleteoAguila) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzquierdaAguila.RenderModel();

		//AGUILA 2: BOSQUE CAZANDO
		//Instancia del cuerpo del águila
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(5.5f + radio * cos(anguloAguila), 5.0f, 5.5f + radio * sin(anguloAguila)));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (-anguloVistaAguila) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAguila.RenderModel();

		//Instancia del ala derecha del águila
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.02f, 0.05f, 0.1f));
		model = glm::rotate(model, (-rangoAleteoAguila) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDerechaAguila.RenderModel();

		//Instancia del ala izquierda del águila
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.02f, 0.05f, 0.1f));
		model = glm::rotate(model, (rangoAleteoAguila)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzquierdaAguila.RenderModel();

		//LOBOS QUE VAN A ESTAR FIJOS EN LAS MONTAÑAS (animaciones no agregadas aún)
		//Instancia del lobo 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-7.5f, 2.85f, 7.7f));
		model = glm::rotate(model, 80 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lobo.RenderModel();

		//Instancia del lobo 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-7.0f, 2.85f, 7.3f));
		model = glm::rotate(model, 70 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lobo.RenderModel();

		//Instancia del lobo 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-6.7f, 2.85f, 7.7f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lobo.RenderModel();


		//Instancia del primer ajolote nadando
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-5.0f, -1.87f, -3.0f - movAjolote1));
		modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 5 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotarAjolote1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAjolote.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, sin(rangoAleteoColaAjolote) / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotarAjolote1 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ColaAjolote.RenderModel();

		//Instancia del segundo ajolote nadando
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-7.0f, -1.87f, -7.5f + movAjolote2));
		modelaux = model;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 5 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotarAjolote2 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAjolote.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, sin(rangoAleteoColaAjolote) / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotarAjolote2 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ColaAjolote.RenderModel();


		//Instancia de los mapaches
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(6.5f, -0.55f, -6.5f));
		model = glm::rotate(model, 225 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mapache.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(6.5f, -0.55f, -7.0f));
		model = glm::rotate(model, 250 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mapache.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(6.8f, -0.55f, -6.0f));
		model = glm::rotate(model, 200 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.12f, 0.12f, 0.12f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mapache.RenderModel();

		
		//INSTANCIAS DE LAS ABEJAS

		//Abejas en flores azules/blancas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(7.38f, -1.54f + sin(movAbejaY) / 10, 8.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzquierdaAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, -rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDerechaAbeja.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(2.6f, -1.5f + sin(movAbejaY) / 10, -8.6f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzquierdaAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, -rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDerechaAbeja.RenderModel();


		//Abejas en flores moradas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(4.5f + radioAbeja*cos(anguloAbeja), -0.9 + sin(movAbejaY) / 10, 0.8f + radioAbeja * sin(anguloAbeja)));
		model = glm::rotate(model, -anguloVistaAguila * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuerpoAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaIzquierdaAbeja.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.004f, 0.0f));
		model = glm::rotate(model, -rangoAleteoAbeja * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AlaDerechaAbeja.RenderModel();

		//Instancia del muro blanco
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-0.4f, -1.88f, -9.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.035f, 0.035f, 0.035f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MuroBlanco.RenderModel();

		tooffsetTU += 0.0001;
		tooffsetTV = 0.0f;
		if (tooffsetTU > 1.0) {
			tooffsetTU = 0.0f;
		}
		toffsetTEXT = glm::vec2(tooffsetTU, tooffsetTV);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-0.4f, -0.58f, -9.59999f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 0.6f, 1.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffsetTEXT));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);										// GL_BLEND para quitar el fondo blanco
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Sin fondo solo dejamos las letras moviéndose
		MAINTEXT.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f - movEscenario1, 0.0f));
		model = glm::translate(model, glm::vec3(-0.4f, -1.38f, -9.59999f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.70f, 0.55f, 0.55f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffsetTEXT));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);										
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		INTEGRANTES.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		glDisable(GL_BLEND);

		//INSTANCIA DE LA PARTE NATURAL Y TODOS SUS MODELOS
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f - movEscenario1, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		escenario_Antes.RenderModel();
		glDisable(GL_BLEND);




		//ESCENARIO ACTUAL
		//Instancia de la ciudad actual
		model = glm::mat4(1.0);
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-10.0f, -1.65f - movEscenario2, -9.5f));
		model = glm::scale(model, glm::vec3(0.0175f, 0.0165f, 0.0165f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		escenario_Durante.RenderModel();
		glDisable(GL_BLEND);




		////ESCENARIO DESPUÉS
		/*model = glm::mat4(1.0);
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-10.0f, -1.65f - movEscenario3, -9.5f));
		model = glm::scale(model, glm::vec3(0.0175f, 0.0165f, 0.0165f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		escenario_Despues.RenderModel();
		glDisable(GL_BLEND);*/

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
