//OpenGL Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//Header definitions
#define STB_IMAGE_IMPLEMENTATION

//CV headers

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//User Headers
#include <Shader.h>
#include <camera.h>
#include <Model.h>
#include <textgl.h>
#include <Framework.h>
#include <lights.h>
#include <stb_image_write.h>
#include <skybox.h>


//GUI Headers
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

//Standard Headers
#include <iostream>
#include <ctime>
#include <Windows.h>


using namespace std;
using namespace cv;


//face location
int cameraWidth = 640;
int cameraHeight = 480;
float facex = 0;
float facey = 0;
CascadeClassifier face_cascade;
VideoCapture capture(0);

//Light Position
glm::vec3 lightPos(0.0f, 0.0f, 3.0f);


// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void playMusic(string const &path);
ALfloat GetBufferLength(ALuint buffer);
unsigned int loadImageGUI(string const &path);
unsigned int loadCubemap(vector<std::string> faces);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4); //to use multisample buffer


#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
																			//glfwGetPrimaryMonitor() for full screen
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Vvargos",NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(1); //VSYNC ENABLER

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);  //enable AA


	// build and compile shaders
	// -------------------------
	//Shader ourShader("VertexShaders\\vertexShader.vs", "FragmentShaders\\fragmentShader.fs");
	Shader ourShader("VertexShaders\\VertexShaderL.vs", "FragmentShaders\\fragmentShaderL.fs"); // Main Shader for objects
	Shader lampShader("VertexShaders\\vertexShadersLamp.vs", "FragmentShaders\\fragmentShaderLamp.fs"); //position lamp shaders
	Shader singleShader("VertexShaders\\vertexShadersLamp.vs", "FragmentShaders\\fragmentShaderSingle.fs"); //outline shaders
	Shader cubeMapShader("VertexShaders\\vertexShaderCubeMap.vs", "FragmentShaders\\fragmentShaderCubeMap.fs"); //cube map shaders

	// load models
	// -----------
	//Model ourModel("Models\\nanosuit\\nanosuit.obj");
	Model ourModel("Models\\Test\\ball.obj");
	Model Lamp("Models\\Test\\ball.obj");

	//Load Lights
	LightPoint mainLight(glm::vec3(0.0f,2.0f,0.0f));
	LightDir dirlight;
	LightSpot spotlight;

	//Text 
	textGL arial("Fonts\\arial.ttf", 48);

	//playMusic("test.wav");

	//Frame Buffer
	/* 
	We have to attach at least one buffer (color, depth or stencil buffer).
	There should be at least one color attachment.
	All attachments should be complete as well (reserved memory).
	Each buffer should have the same number of samples.
	*/
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	
	
	//Background Image
	unsigned int backgroundImage = loadImageGUI("resources\\Background Images\\wave.jpg");

	//Icons Test
	unsigned int fileImage = loadImageGUI("resources\\Icons\\file.png");

	//Cube Map
	SkyBox skybox;

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	float start = glfwGetTime();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	while (!glfwWindowShouldClose(window))
	{


		glm::mat4 model(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();


		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);// Face Culling
							   //glCullFace(GL_BACK); //
							   //glFrontFace(GL_CW); //front face defualt ccw
		glEnable(GL_STENCIL_TEST);



		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Scene
		
		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations

		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);


		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		// render the loaded model
		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::scale(model, glm::vec3(.7f));
			ourShader.setMat4("model", model);
			ourShader.setVec3("viewPos", camera.Position);
			ourShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
			ourShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
			ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
			ourShader.setFloat("material.shininess", 8.0f);
			ourModel.Draw(ourShader);
		}

		//Light Rendering
		spotlight.setPosition(camera.Position);
		spotlight.setDirection(camera.Front);
		spotlight.setDiffuse(glm::vec3(0.9));
		spotlight.use(ourShader);
		mainLight.use(ourShader);


		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		singleShader.use();
		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::scale(model, glm::vec3(.73f));
			singleShader.setMat4("model", model);
			singleShader.setMat4("projection", projection);
			singleShader.setMat4("view", view);
			ourModel.Draw(singleShader);
		}

		//Cube Map(Skybox) 
		cubeMapShader.use();
		cubeMapShader.setMat4("projection", projection);
		cubeMapShader.setMat4("view", glm::mat4(glm::mat3(view)));
		skybox.use(cubeMapShader);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF); //enables lighting to pass depth tests and stencil tests
		//glStencilMask(0x00);
		//Lamp
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));

		lampShader.setMat4("model", model);
		Lamp.Draw(lampShader);

		processInput(window);
		//arial.RenderText("FPS: " + to_string((int)(1 / deltaTime)), 25.f, 25.0f, 1.0f, glm::vec3(0.0f, 0.8f, 0.2f), SCR_WIDTH, SCR_HEIGHT);

		
		


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

ALfloat GetBufferLength(ALuint buffer)
{
	ALint size, bits, channels, freq;

	alGetBufferi(buffer, AL_SIZE, &size);
	alGetBufferi(buffer, AL_BITS, &bits);
	alGetBufferi(buffer, AL_CHANNELS, &channels);
	alGetBufferi(buffer, AL_FREQUENCY, &freq);
	if (alGetError() != AL_NO_ERROR)
		return -1.0f;

	return (ALfloat)((ALuint)size / channels / (bits / 8)) / (ALfloat)freq;
}

void playMusic(string const &path) {
	ALCdevice *device;
	ALCcontext *context;
	ALuint      uiBuffer;
	ALuint      uiSource;
	ALint       iState;

	ALFWInit();
	
	device = alcOpenDevice(NULL);
	if (!device) {
		ALFWShutdown();
		return;
	}

	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		ALFWShutdown();
		return;
	}


	alGenBuffers(1, &uiBuffer);

	if (!ALFWLoadWaveToBuffer(path.c_str(), uiBuffer)) {
		ALFWprintf("Failed to Load %s\n", path.c_str());
	}

	alGenSources(1, &uiSource);

	alSourcei(uiSource, AL_BUFFER, uiBuffer);

	alSourcePlay(uiSource);
	ALFWprintf("Playing Source ");
	


}

unsigned int loadImageGUI(string const &path) {
	int width;
	int height;
	int nrChannels;
	unsigned int textureID;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{	
	int width;
	int height;
	int nrChannels;
	unsigned int textureID;
	stbi_set_flip_vertically_on_load(false);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}
