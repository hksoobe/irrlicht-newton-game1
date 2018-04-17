/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "toolbox_stdafx.h"
#include "SkyBox.h"
#include "DemoMesh.h"
#include "DemoEntity.h"
#include "DemoCamera.h"
#include "PhysicsUtils.h"
#include "DebugDisplay.h"
#include "TargaToOpenGl.h"
#include "DemoEntityManager.h"
#include "DemoCameraManager.h"
#include "DemoEntityListener.h"
#include "DemoCameraManager.h"
#include "dHighResolutionTimer.h"

#ifdef _MACOSX_VER
	#include "CocoaOpenglGlue.h"
#endif

#define MAX_PHYSICS_FPS				60.0f
#define MAX_PHYSICS_SUB_STEPS		2
#define PROJECTILE_INITIAL_SPEED	20.0f

//#define DEFAULT_SCENE	0			// using NetwonMesh Tool
//#define DEFAULT_SCENE	1			// Coefficients of friction
//#define DEFAULT_SCENE	2			// Coefficients of restitution
//#define DEFAULT_SCENE	3			// Precessing tops
//#define DEFAULT_SCENE	4			// closest distance
//#define DEFAULT_SCENE	5			// primitive collision
//#define DEFAULT_SCENE	6 			// Kinematic bodies
//#define DEFAULT_SCENE	7			// primitive convex cast 
//#define DEFAULT_SCENE	8			// Box stacks
//#define DEFAULT_SCENE	9			// simple level mesh collision
//#define DEFAULT_SCENE	10			// optimized level mesh collision
//#define DEFAULT_SCENE	11			// height field Collision
//#define DEFAULT_SCENE	12			// infinite user plane collision
//#define DEFAULT_SCENE	13			// user height field Collision
//#define DEFAULT_SCENE	14			// compound Collision
//#define DEFAULT_SCENE	15			// simple Archimedes buoyancy
//#define DEFAULT_SCENE	16			// uniform Scaled Collision
//#define DEFAULT_SCENE	17			// non uniform Scaled Collision
//#define DEFAULT_SCENE	18			// scaled mesh collision
//#define DEFAULT_SCENE	19			// continuous collision
//#define DEFAULT_SCENE	20			// paper wall continuous collision
//#define DEFAULT_SCENE	21			// puck slide continuous collision
//#define DEFAULT_SCENE	22			// simple convex decomposition
//#define DEFAULT_SCENE	23			// scene Collision
//#define DEFAULT_SCENE	24          // simple boolean operators 
//#define DEFAULT_SCENE	25			// simple convex fracturing 
//#define DEFAULT_SCENE	26			// structured convex fracturing 
//#define DEFAULT_SCENE	27			// multi ray casting using the threading Job scheduler
//#define DEFAULT_SCENE	28          // standard joints
//#define DEFAULT_SCENE	29			// six axis manipulator
#define DEFAULT_SCENE	30			// hexapod Robot
//#define DEFAULT_SCENE	31			// articulated joints
//#define DEFAULT_SCENE	32			// basic rag doll
//#define DEFAULT_SCENE	33			// dynamics rag doll
//#define DEFAULT_SCENE	34			// basic Car
//#define DEFAULT_SCENE	35			// super Car
//#define DEFAULT_SCENE	36			// heavy vehicles
//#define DEFAULT_SCENE	37			// basic player controller
//#define DEFAULT_SCENE	38			// advanced player controller
//#define DEFAULT_SCENE	39			// cloth patch			
//#define DEFAULT_SCENE	40			// soft bodies			


/// demos forward declaration 
void Friction (DemoEntityManager* const scene);
void Restitution (DemoEntityManager* const scene);
void PrecessingTops (DemoEntityManager* const scene);
void ClosestDistance (DemoEntityManager* const scene);
void ConvexCast (DemoEntityManager* const scene);
void PrimitiveCollision (DemoEntityManager* const scene);
void KinematicPlacement (DemoEntityManager* const scene);
void ClothPatch(DemoEntityManager* const scene);
void SoftBodies (DemoEntityManager* const scene);
void BasicBoxStacks (DemoEntityManager* const scene);
void SimpleMeshLevelCollision (DemoEntityManager* const scene);
void OptimizedMeshLevelCollision (DemoEntityManager* const scene);
void UniformScaledCollision (DemoEntityManager* const scene);
void NonUniformScaledCollision (DemoEntityManager* const scene);
void ScaledMeshCollision (DemoEntityManager* const scene);
void ContinuousCollision (DemoEntityManager* const scene);
void ContinuousCollision1 (DemoEntityManager* const scene);
void PuckSlide (DemoEntityManager* const scene);
void SceneCollision (DemoEntityManager* const scene);
void CompoundCollision(DemoEntityManager* const scene);
void AlchimedesBuoyancy(DemoEntityManager* const scene);
void SimpleConvexApproximation(DemoEntityManager* const scene);
void SimpleBooleanOperations(DemoEntityManager* const scene);
void SimpleConvexFracturing (DemoEntityManager* const scene);
void StructuredConvexFracturing (DemoEntityManager* const scene);
void UsingNewtonMeshTool (DemoEntityManager* const scene);
void MultiRayCast (DemoEntityManager* const scene);
void BasicCar (DemoEntityManager* const scene);
void SuperCar (DemoEntityManager* const scene);
void MilitaryTransport (DemoEntityManager* const scene);
void BasicPlayerController (DemoEntityManager* const scene);
void AdvancedPlayerController (DemoEntityManager* const scene);
void HeightFieldCollision (DemoEntityManager* const scene);
void UserPlaneCollision (DemoEntityManager* const scene);
void UserHeightFieldCollision (DemoEntityManager* const scene);
void PassiveRagdoll (DemoEntityManager* const scene);
void DynamicRagDoll (DemoEntityManager* const scene);
void ArticulatedJoints (DemoEntityManager* const scene);
void StandardJoints (DemoEntityManager* const scene);
void SixAxisManipulators(DemoEntityManager* const scene);
void Hexapod(DemoEntityManager* const scene);


DemoEntityManager::SDKDemos DemoEntityManager::m_demosSelection[] = 
{
	{"Using the newton mesh tool", "demonstrate how to use the newton mesh tool for mesh manipulation", UsingNewtonMeshTool},
	{"Coefficients of friction", "demonstrate the effect of various coefficient of friction", Friction},
	{"Coefficients of restitution", "demonstrate the effect of various coefficient of restitution", Restitution},
	{"Precessing tops", "show natural precession", PrecessingTops},
	{"Closest distance", "demonstrate closest distance to a convex shape", ClosestDistance},
	{"Primitive Collision", "demonstrate separate collision of primitives", PrimitiveCollision},
	{"Kinematic bodies", "demonstrate separate collision of primitives", KinematicPlacement},
	{"Primitive convex cast", "demonstrate separate primitive convex cast", ConvexCast},
	{"Simple box Stacks", "show simple stack of Boxes", BasicBoxStacks},
	{"Unoptimized mesh collision", "show simple level mesh", SimpleMeshLevelCollision},
	{"Optimized mesh collision", "show optimized level mesh", OptimizedMeshLevelCollision},
	{"Height field collision mesh", "show high file collision mesh", HeightFieldCollision},
	{"User infinite Plane collision mesh", "show high file collision mesh", UserPlaneCollision},
	{"User Height field collision mesh", "show high file collision mesh", UserHeightFieldCollision},
	{"Compound collision shape", "demonstrate compound collision", CompoundCollision},
	{"Archimedes Buoyancy", "show Archimedes Buoyancy using the trigger volume manager", AlchimedesBuoyancy},
	{"Uniform scaled collision shape", "demonstrate scaling shape", UniformScaledCollision},
	{"Non uniform scaled collision shape", "demonstrate scaling shape", NonUniformScaledCollision},
	{"Scaled mesh collision", "demonstrate scaling mesh scaling collision", ScaledMeshCollision},
	{"Continuous collision", "show continuous collision", ContinuousCollision },
	{"Paper wall continuous collision", "show fast continuous collision", ContinuousCollision1 },
	{"Puck slide", "show continuous collision", PuckSlide },
	{"Simple convex decomposition", "demonstrate convex decomposition and compound collision", SimpleConvexApproximation},
	{"Multi geometry collision", "show static mesh with the ability of moving internal parts", SceneCollision},
	{"Simple boolean operations", "demonstrate simple boolean operations ", SimpleBooleanOperations},
	{"Simple convex fracture", "demonstrate simple fracture destruction using Voronoi partition", SimpleConvexFracturing},
	{"Structured convex fracture", "demonstrate structured fracture destruction using Voronoi partition", StructuredConvexFracturing},
	{"Parallel ray cast", "using the threading Job scheduler", MultiRayCast},
	{"Standard Joints", "show some of the common joints", StandardJoints},
	{"Six axis manipulator", "show using inverse dynamics to control robots", SixAxisManipulators },
	{"Hexapod walker", "show using inverse dynamics to control robots", Hexapod },
	{"Articulated robotic actuators joints", "demonstrate complex array of bodies interconnect by joints", ArticulatedJoints},
	{"Passive rag doll", "demonstrate passive rag doll", PassiveRagdoll},
	{"Dynamic rag doll", "demonstrate dynamic rag doll", DynamicRagDoll},
	{"Basic Car", "show how to set up a vehicle controller", BasicCar},
	{"Super car", "implement a hight performance sport car", SuperCar},
	{"Heavy vehicles", "implement military type heavy Vehicles", MilitaryTransport},
	{"Basic player controller", "demonstrate simple player controller", BasicPlayerController},
	{"Advanced player controller", "demonstrate player interacting with other objects", AdvancedPlayerController},
	{"Simple cloth Patch", "show simple cloth patch", ClothPatch},
	{"Simple soft Body", "show simple soft body", SoftBodies},
};


DemoEntityManager::ButtonKey::ButtonKey (bool state)
	:m_state(state)
	,m_memory0(false)
	,m_memory1(false)
{
}

int DemoEntityManager::ButtonKey::UpdateTrigger (bool triggerValue)
{
	m_memory0 = m_memory1;
	m_memory1 = triggerValue;
	return (!m_memory0 & m_memory1) ? 1 : 0;
}

int DemoEntityManager::ButtonKey::UpdatePushButton (bool triggerValue)
{
	if (UpdateTrigger (triggerValue)) {
		m_state = ! m_state;
	}
	return m_state ? 1 : 0;
}


// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
DemoEntityManager::DemoEntityManager ()
	:m_mainFrame(NULL)
	,m_defaultFont(0)
	,m_sky(NULL)
	,m_world(NULL)
	,m_cameraManager(NULL)
	,m_renderUIContext(NULL)
	,m_updateCameraContext(NULL)
	,m_renderDemoGUI(NULL)
	,m_renderHelpMenus(NULL)
	,m_updateCamera(NULL)
	,m_microsecunds(0)
	,m_tranparentHeap()
	,m_currentScene(DEFAULT_SCENE)
	,m_framesCount(0)
	,m_physicsFramesCount(0)
	,m_fps(0.0f)
	,m_timestepAcc(0.0f)
	,m_currentListenerTimestep(0.0f)
	,m_mainThreadPhysicsTime(0.0f)
	,m_mainThreadPhysicsTimeAcc(0.0f)
	,m_broadPhaseType(0)
	,m_workerThreades(1)
	,m_solverPasses(4)
	,m_debugDisplayMode(0)
	,m_collisionDisplayMode(0)
	,m_showUI(true)
	,m_showAABB(false)
	,m_showStats(true)
	,m_hasJoytick(false)
	,m_autoSleepMode(true)
	,m_hideVisualMeshes(false)
	,m_showNormalForces(false)
	,m_showCenterOfMass(false)
	,m_updateMenuOptions(true)
	,m_showContactPoints(false)
	,m_showJointDebugInfo(false)
	,m_suspendPhysicsUpdate(false)
	,m_asynchronousPhysicsUpdate(false)
{
//	m_showUI = true;
//	m_showAABB = false;
//	m_showContactPoints = false;
//	m_hideVisualMeshes = false;
//	m_autoSleepMode = true;
//	m_broadPhaseType = 1;
//	m_solverPasses = 4;
//	m_workerThreades = 1;
//	m_showNormalForces = false;
//	m_showCenterOfMass = false;
//	m_showJointDebugInfo = true;
//	m_collisionDisplayMode = 2;
//	m_synchronousPhysicsUpdateMode = false;
	// Setup window
	glfwSetErrorCallback(ErrorCallback);

	glfwInit();

	m_hasJoytick = glfwJoystickPresent(0) ?  true : false;

	m_mainFrame = glfwCreateWindow(1280, 720, "Newton Game Dynamics 3.14 demos", NULL, NULL);
	glfwMakeContextCurrent(m_mainFrame);

	int monitorsCount;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);
	if (monitorsCount > 1) {
		int window_x;
		int window_y;
		int monitor_x;
		int monitor_y;

		glfwGetMonitorPos(monitors[1], &monitor_x, &monitor_y);
		glfwGetWindowPos(m_mainFrame, &window_x, &window_y);
		glfwSetWindowPos(m_mainFrame, monitor_x + window_x, monitor_y + 64);
	}

	// attach myself to the main frame
	glfwSetWindowUserPointer(m_mainFrame, this);

	// Setup ImGui binding
	ImGuiIO& io = ImGui::GetIO();
	io.UserData = this;

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                     
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	// Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.RenderDrawListsFn = RenderDrawListsCallback;
	//	io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
	//	io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;

#ifdef _MSC_VER 
	io.ImeWindowHandle = glfwGetWin32Window(m_mainFrame);
#else 
	dAssert (0);
#endif

	glfwSwapInterval(0);
	glfwSetKeyCallback(m_mainFrame, KeyCallback);
	glfwSetScrollCallback(m_mainFrame, MouseScrollCallback);
	glfwSetCursorPosCallback(m_mainFrame, CursorposCallback);
	glfwSetMouseButtonCallback(m_mainFrame, MouseButtonCallback);
	//glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
	//glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);

	LoadFont();

	m_mousePressed[0] = false;
	m_mousePressed[1] = false;
	m_mousePressed[2] = false;

	// initialized the physics world for the new scene
	Cleanup ();
	ResetTimer();

/*
	dFloat A[2][2];
	dFloat x[2];
	dFloat b[2];
	dFloat l[2];
	dFloat h[2];

	A[0][0] = 2.0f;
	A[0][1] = 1.0f;
	A[1][0] = 1.0f;
	A[1][1] = 2.0f;
	b[0] = 1.0f;
	b[1] = 1.0f;
	x[0] = 1;
	x[1] = 2;
	
	l[0] = 0.0f;
	l[1] = 0.0f;
	h[0] = 0.25f;
	h[1] = 1.0f;
	
	dMatrixTimeVector(2, &A[0][0], x, b);
	dSolveDantzigLCP(2, &A[0][0], x, b, l, h);
*/
}



DemoEntityManager::~DemoEntityManager ()
{
	// is we are run asynchronous we need make sure no update in on flight.
	if (m_world) {
		NewtonWaitForUpdateToFinish (m_world);
	}

	Cleanup ();

	// destroy the empty world
	if (m_world) {
		NewtonDestroy (m_world);
		m_world = NULL;
	}

	// Cleanup
	GLuint font_texture (m_defaultFont);
	glDeleteTextures(1, &font_texture);
	ImGui::GetIO().Fonts->TexID = 0;

	ImGui::Shutdown();

	dAssert (NewtonGetMemoryUsed () == 0);
}


DemoCamera* DemoEntityManager::GetCamera() const
{
	return m_cameraManager->GetCamera();
}


bool DemoEntityManager::GetKeyState(int key) const
{
	const ImGuiIO& io = ImGui::GetIO();
	return io.KeysDown[key];
}

bool DemoEntityManager::IsShiftKeyDown () const
{
	const ImGuiIO& io = ImGui::GetIO();
	bool state = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	return state;
}

bool DemoEntityManager::IsControlKeyDown () const
{
	const ImGuiIO& io = ImGui::GetIO();
	bool state = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	return state;
}

bool DemoEntityManager::GetMouseKeyState (int button) const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.MouseDown[button];
}

void DemoEntityManager::Set2DDisplayRenderFunction (RenderGuiHelpCallback helpCallback, RenderGuiHelpCallback UIcallback, void* const context)
{
	m_renderDemoGUI = UIcallback;
	m_renderHelpMenus = helpCallback;
	m_renderUIContext = context;
}

void DemoEntityManager::SetUpdateCameraFunction(UpdateCameraCallback callback, void* const context)
{
	m_updateCamera = callback;
	m_updateCameraContext = context;
}

int DemoEntityManager::GetJoystickAxis (dFloat* const axisValues, int maxAxis) const
{
	int axisCount = 0;
	if (m_hasJoytick) {
		const float* const axis = glfwGetJoystickAxes(0, &axisCount);
		axisCount = dMin (axisCount, maxAxis);
		for (int i = 0; i < axisCount; i ++) {
			axisValues[i] = axis[i];
		}
	}
	return axisCount;
}

int DemoEntityManager::GetJoystickButtons (char* const axisbuttons, int maxButton) const
{
	int buttonsCount = 0;
	if (m_hasJoytick) {
		const unsigned char* const buttons = glfwGetJoystickButtons(0, &buttonsCount);
		buttonsCount = dMin (buttonsCount, maxButton);
		for (int i = 0; i < buttonsCount; i ++) {
			axisbuttons[i] = buttons[i];
		}
	}
	return buttonsCount;
}


void DemoEntityManager::ResetTimer()
{
	dResetTimer();
	m_microsecunds = dGetTimeInMicrosenconds ();
}

void DemoEntityManager::RemoveEntity (dListNode* const entNode)
{
	DemoEntity* const entity = entNode->GetInfo();
	entity->Release();
	Remove(entNode);
}

void DemoEntityManager::RemoveEntity (DemoEntity* const ent)
{
	for (dListNode* node = dList<DemoEntity*>::GetFirst(); node; node = node->GetNext()) {
		if (node->GetInfo() == ent) {
			RemoveEntity (node);
			break;
		}
	}
}


void DemoEntityManager::Cleanup ()
{
	// is we are run asynchronous we need make sure no update in on flight.
	if (m_world) {
		NewtonWaitForUpdateToFinish (m_world);
	}

	// destroy all remaining visual objects
	while (dList<DemoEntity*>::GetFirst()) {
		RemoveEntity (dList<DemoEntity*>::GetFirst());
	}

	m_sky = NULL;
	m_updateCamera = NULL;

	// destroy the Newton world
	if (m_world) {
		// get serialization call back before destroying the world
		NewtonDestroy (m_world);
		m_world = NULL;
	}

	// check that there are no memory leak on exit
	dAssert (NewtonGetMemoryUsed () == 0);

	// create the newton world
	m_world = NewtonCreate();

	// set a post update callback which is call after all simulation and all listeners updates
	NewtonSetPosUpdateCallback (m_world, PostUpdateCallback);

	// link the work with this user data
	NewtonWorldSetUserData(m_world, this);

	// set joint serialization call back
	dCustomJoint::Initalize(m_world);

	// add all physics pre and post listeners
	new DemoEntityListener (this);

	// add the camera manager
	m_cameraManager = new DemoCameraManager(this);

	ApplyMenuOptions();

	// Set the Newton world user data
	NewtonWorldSetUserData(m_world, this);

	// set the number of sub steps
	NewtonSetNumberOfSubsteps (m_world, MAX_PHYSICS_SUB_STEPS);

	// for debugging time spend on phys update
	NewtonSetPerformanceClock (m_world, dGetTimeInMicrosenconds);

	// we start without 2d render
	m_renderDemoGUI = NULL;
	m_renderHelpMenus = NULL;
	m_renderUIContext = NULL;
}


void DemoEntityManager::LoadFont()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //io.Fonts->AddFontDefault();

	float pixedSize = 18;
	char pathName[2048];
	char* const name = "Cousine-Regular.ttf";
	//char* const name = "calibri.ttf";
	//char* const name = "courbd.ttf";

	dGetWorkingFileName (name, pathName);
    io.Fonts->AddFontFromFileTTF(pathName, pixedSize);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. 
	// If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   

	// Upload texture to graphics system
	GLint last_texture;
	GLuint font_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	m_defaultFont = int (font_texture);
	io.Fonts->TexID = (void *)(intptr_t)m_defaultFont;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

void DemoEntityManager::ApplyMenuOptions()
{
	NewtonWaitForUpdateToFinish(m_world);

	// clean up all caches the engine have saved
	NewtonInvalidateCache(m_world);
	NewtonSetSolverModel(m_world, m_solverPasses);
	NewtonSetThreadsCount(m_world, m_workerThreades);

	int state = m_autoSleepMode ? 1 : 0;
	for (const NewtonBody* body = NewtonWorldGetFirstBody(m_world); body; body = NewtonWorldGetNextBody(m_world, body)) {
		NewtonBodySetAutoSleep(body, state);
	}

	NewtonSelectBroadphaseAlgorithm(m_world, m_broadPhaseType);
	//NewtonSetMultiThreadSolverOnSingleIsland (m_scene->m_world, m_useParallelSolver ? 1 : 0);	
}

void DemoEntityManager::ShowMainMenuBar()
{
	int mainMenu = 0;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File")) {
			m_suspendPhysicsUpdate = true;

			if (ImGui::MenuItem("About", "")) {
				dAssert (0);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Preferences", "")) {
				dAssert (0);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("New", "")) {
				mainMenu = 1;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Open", "")) {
				dAssert (0);
			}
			if (ImGui::MenuItem("Save", "")) {
				dAssert (0);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Serialize", "")) {
				//mainMenu = 2;
				dAssert (0);
			}
			if (ImGui::MenuItem("Deserialize", "")) {
				dAssert (0);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "")) {
				glfwSetWindowShouldClose (m_mainFrame, 1);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Demos")) {
			m_suspendPhysicsUpdate = true;
			int demosCount = int (sizeof (m_demosSelection) / sizeof m_demosSelection[0]);
			for (int i = 0; i < demosCount; i ++) {
				if (ImGui::MenuItem(m_demosSelection[i].m_name, "")) {
					m_currentScene = i;
				}
			}

			ImGui::EndMenu();
		}

		bool optionsOn = ImGui::BeginMenu("Options");
		if (optionsOn) {
			m_updateMenuOptions = true;
			m_suspendPhysicsUpdate = true;

			ImGui::Checkbox("Auto Sleep Mode", &m_autoSleepMode);
			ImGui::Checkbox("Show UI", &m_showUI);
			ImGui::Checkbox("Show stats", &m_showStats);
			ImGui::Checkbox("concurrent physics update", &m_asynchronousPhysicsUpdate);
			ImGui::Separator();

			ImGui::RadioButton("default broad phase", &m_broadPhaseType, 0);
			ImGui::RadioButton("persistence broad phase", &m_broadPhaseType, 1);
			ImGui::Separator();

			ImGui::RadioButton("Iterative solver one passes", &m_solverPasses, 1);
			ImGui::RadioButton("Iterative solver tow passes", &m_solverPasses, 2);
			ImGui::RadioButton("Iterative solver four passes", &m_solverPasses, 4);
			ImGui::RadioButton("Iterative solver eight passes", &m_solverPasses, 8);
			ImGui::Separator();

			ImGui::RadioButton("Hide collision Mesh", &m_collisionDisplayMode, 0);
			ImGui::RadioButton("Show solid collision Mesh", &m_collisionDisplayMode, 1);
			ImGui::RadioButton("Show wire frame collision Mesh", &m_collisionDisplayMode, 2);
			ImGui::Separator();

			ImGui::Checkbox("Show aabb", &m_showAABB);
			ImGui::Checkbox("Hide visual meshes", &m_hideVisualMeshes);
			ImGui::Checkbox("Show contact points", &m_showContactPoints);
			ImGui::Checkbox("Show normal forces", &m_showNormalForces);
			ImGui::Checkbox("Show center of mass", &m_showCenterOfMass);
			ImGui::Checkbox("show Joint debug info", &m_showJointDebugInfo);
			ImGui::Separator();
			
			ImGui::Text ("select worker threads");
			ImGui::RadioButton("one", &m_workerThreades, 1);
			ImGui::RadioButton("two", &m_workerThreades, 2);
			ImGui::RadioButton("three", &m_workerThreades, 3);
			ImGui::RadioButton("four", &m_workerThreades, 4);
			ImGui::RadioButton("eight", &m_workerThreades, 8);
			ImGui::RadioButton("sixteen", &m_workerThreades, 16);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			m_suspendPhysicsUpdate = true;
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if (!optionsOn && m_updateMenuOptions) {
			m_updateMenuOptions = false;
			ApplyMenuOptions();
		}
	}

	switch (mainMenu)
	{
		case 1:
		{
			Cleanup();
			ApplyMenuOptions();
			ResetTimer();
			m_currentScene = -1;
			break;
		}

		case 2:
		{
			Cleanup();
			ApplyMenuOptions();
			ResetTimer();
			m_currentScene = -1;
			break;
		}

		default:
		{
			if (m_currentScene != -1) {
				Cleanup();
				m_demosSelection[m_currentScene].m_launchDemoCallback(this);
				ApplyMenuOptions();
				ResetTimer();
				m_currentScene = -1;
			}
		}
	}
}


void DemoEntityManager::ErrorCallback(int error, const char* description)
{
	dAssert (0);
	dTrace (("Error %d: %s\n", error, description));
	fprintf(stderr, "Error %d: %s\n", error, description);
}


void DemoEntityManager::MouseButtonCallback(GLFWwindow*, int button, int action, int)
{
	if (button >= 0 && button < 3) {
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS) {
			io.MouseDown[button] = true;    
		} else if (action == GLFW_RELEASE) {
			io.MouseDown[button] = false;    
		}
	}
}

void DemoEntityManager::MouseScrollCallback(GLFWwindow* const window, double x, double y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += float (y);
}

void DemoEntityManager::CursorposCallback  (GLFWwindow* , double x, double y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x, (float)y);
}

bool DemoEntityManager::GetMousePosition (int& posX, int& posY) const
{
	const ImGuiIO& io = ImGui::GetIO();
	posX = int (io.MousePos.x);
	posY = int (io.MousePos.y);
	return true;
}




void DemoEntityManager::KeyCallback(GLFWwindow* const window, int key, int, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose (window, 1);
	}
}


void DemoEntityManager::BeginFrame()
{
	glfwPollEvents();
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(m_mainFrame, &w, &h);
	glfwGetFramebufferSize(m_mainFrame, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Start the frame
	ImGui::NewFrame();
}

void DemoEntityManager::RenderStats()
{
	if (m_showStats) {
		char text[1024];
		
		if (ImGui::Begin("statistics", &m_showStats)) {
			sprintf (text, "fps:           %6.3f", m_fps);
			ImGui::Text(text);

			sprintf (text, "physics time: %6.3f ms", m_mainThreadPhysicsTime * 1000.0f);
			ImGui::Text(text);

			sprintf (text, "memory used:   %d kbytes", NewtonGetMemoryUsed() / 1024);
			ImGui::Text(text);

			sprintf (text, "threads:       %d", NewtonGetThreadsCount(m_world));
			ImGui::Text(text);

			sprintf (text, "bodies:        %d", NewtonWorldGetBodyCount(m_world));
			ImGui::Text(text);

/*
			wxString statusText;
			NewtonWorld* const world = m_world;
			char platform[256];
			NewtonGetDeviceString(world, NewtonGetCurrentDevice(world), platform, sizeof (platform));

			char floatMode[128];
			NewtonGetDeviceString (world, m_hardwareDevice, floatMode, sizeof (floatMode));
			statusText.Printf (wxT ("instructions: %s",  wxString::FromAscii(floatMode).wc_str());
			m_statusbar->SetStatusText (statusText, 6);
*/
			m_suspendPhysicsUpdate = m_suspendPhysicsUpdate || (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseDown(0));  
			ImGui::End();
		}
	}

	if (m_showUI && m_renderHelpMenus) {
		if (ImGui::Begin("User Interface", &m_showUI)){
			m_renderHelpMenus (this, m_renderUIContext);
			//m_suspendPhysicsUpdate = m_suspendPhysicsUpdate || (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseDown(0));  
			ImGui::End();
		}
	}

	ShowMainMenuBar();
}

void DemoEntityManager::CalculateFPS(dFloat timestep)
{
	m_framesCount ++;
	m_timestepAcc += timestep;

	// this probably happing on loading of and a pause, just rest counters
	if ((m_timestepAcc <= 0.0f) || (m_timestepAcc > 2.0f)){
		m_timestepAcc = 0;
		m_framesCount = 0;
	}

	//update fps every quarter of a second
	if (m_timestepAcc >= 0.25f) {
		m_fps = dFloat (m_framesCount) / m_timestepAcc;
		m_timestepAcc -= 0.25f;
		m_framesCount = 0;
	}
}


void DemoEntityManager::CreateSkyBox()
{
	if (!m_sky) {
		m_sky = new SkyBox();
		Append(m_sky);
	}
}


void DemoEntityManager::PushTransparentMesh (const DemoMeshInterface* const mesh)
{
	dMatrix matrix;
	glGetFloat (GL_MODELVIEW_MATRIX, &matrix[0][0]);
	TransparentMesh entry (matrix, (DemoMesh*) mesh);
	m_tranparentHeap.Push (entry, matrix.m_posit.m_z);
}


void DemoEntityManager::LoadVisualScene(dScene* const scene, EntityDictionary& dictionary)
{
	// load all meshes into a Mesh cache for reuse
	dTree<DemoMeshInterface*, dScene::dTreeNode*> meshDictionary;
	for (dScene::dTreeNode* node = scene->GetFirstNode (); node; node = scene->GetNextNode (node)) {
		dNodeInfo* info = scene->GetInfoFromNode(node);
		if (info->GetTypeId() == dMeshNodeInfo::GetRttiType()) {
			DemoMeshInterface* const mesh = new DemoMesh(scene, node);
			meshDictionary.Insert(mesh, node);
		}
	}

	// create an entity for every root node in the mesh
	// a root node or scene entity is a dSceneNodeInfo with a direct link to the root of the dScene node.
	dScene::dTreeNode* const root = scene->GetRootNode();
	for (void* child = scene->GetFirstChildLink(root); child; child = scene->GetNextChildLink (root, child)) {
		dScene::dTreeNode* node = scene->GetNodeFromLink(child);
		dNodeInfo* info = scene->GetInfoFromNode(node);
		if (info->GetTypeId() == dSceneNodeInfo::GetRttiType()) {
			// we found a root dSceneNodeInfo, convert it to a Scene entity and load all it children 
			DemoEntity* const entityRoot = new DemoEntity (*this, scene, node, meshDictionary, dictionary);
			Append(entityRoot);
		}
	}

	// release all meshes before exiting
	dTree<DemoMeshInterface*, dScene::dTreeNode*>::Iterator iter (meshDictionary);
	for (iter.Begin(); iter; iter++) {
		DemoMeshInterface* const mesh = iter.GetNode()->GetInfo();
		mesh->Release();
	}
}


void DemoEntityManager::LoadScene (const char* const fileName)
{
	dScene database (m_world);

	database.Deserialize(fileName);

	// this will apply all global the scale to the mesh
	database.FreezeScale();
	// this will apply all local scale and transform to the mesh
	//database.FreezePivot();

	// Load the Visual Scene
	EntityDictionary entDictionary;
	LoadVisualScene(&database, entDictionary);

	//Load the physics world
	dList<NewtonBody*> bodyList;
	database.SceneToNewtonWorld(m_world, bodyList);

	// bind every rigidBody loaded to the scene entity
	for (dList<NewtonBody*>::dListNode* bodyNode = bodyList.GetFirst(); bodyNode; bodyNode = bodyNode->GetNext()) {
		// find the user data and set to the visual entity in the scene
		NewtonBody* const body = bodyNode->GetInfo();
		dScene::dTreeNode* const sceneNode = (dScene::dTreeNode*)NewtonBodyGetUserData(body);
		DemoEntity* const entity = entDictionary.Find(sceneNode)->GetInfo();
		NewtonBodySetUserData(body, entity);

		// see if this body have some special setups
		dScene::dTreeNode* const node = database.FindChildByType(sceneNode, dRigidbodyNodeInfo::GetRttiType());
		dAssert (node);
		dRigidbodyNodeInfo* const bodyData = (dRigidbodyNodeInfo*) database.GetInfoFromNode(node);
		dVariable* bodyType = bodyData->FindVariable("rigidBodyType");

		// set the default call backs
		if (!bodyType || !strcmp (bodyType->GetString(), "default gravity")) {
			NewtonBodySetTransformCallback(body, DemoEntity::TransformCallback);
			NewtonBodySetForceAndTorqueCallback(body, PhysicsApplyGravityForce);
			NewtonBodySetDestructorCallback (body, PhysicsBodyDestructor);
		}
	}

	// clean up all caches the engine have saved
	NewtonInvalidateCache (m_world);
}


void DemoEntityManager::SerializeFile (void* const serializeHandle, const void* const buffer, int size)
{
	// check that each chunk is a multiple of 4 bytes, this is useful for easy little to big Indian conversion
	dAssert ((size & 0x03) == 0);
	fwrite (buffer, size, 1, (FILE*) serializeHandle);
}

void DemoEntityManager::DeserializeFile (void* const serializeHandle, void* const buffer, int size)
{
	// check that each chunk is a multiple of 4 bytes, this is useful for easy little to big Indian conversion
	dAssert ((size & 0x03) == 0);
	fread (buffer, size, 1, (FILE*) serializeHandle);
}


void DemoEntityManager::BodySerialization (NewtonBody* const body, void* const bodyUserData, NewtonSerializeCallback serializeCallback, void* const serializeHandle)
{
	// here the use can save information of this body, ex:
	// a string naming the body,  
	// serialize the visual mesh, or save a link to the visual mesh
	// save labels for looking up the body call backs

	// for the demos I will simple write three stream to identify what body it is, the application can do anything
	const char* const bodyIndentification = "gravityBody\0\0\0\0";
	int size = (strlen (bodyIndentification) + 3) & -4;
	serializeCallback (serializeHandle, &size, sizeof (size));
	serializeCallback (serializeHandle, bodyIndentification, size);
}


void DemoEntityManager::BodyDeserialization (NewtonBody* const body, void* const bodyUserData, NewtonDeserializeCallback deserializecallback, void* const serializeHandle)
{
	int size;
	char bodyIndentification[256];

	deserializecallback (serializeHandle, &size, sizeof (size));
	deserializecallback (serializeHandle, bodyIndentification, size);

	// get the world and the scene form the world user data
	NewtonWorld* const world = NewtonBodyGetWorld(body);
	DemoEntityManager* const scene = (DemoEntityManager*)NewtonWorldGetUserData(world);

	// here we attach a visual object to the entity, 
	dMatrix matrix;
	NewtonBodyGetMatrix(body, &matrix[0][0]);
	DemoEntity* const entity = new DemoEntity(matrix, NULL);
	scene->Append (entity);

	NewtonBodySetUserData (body, entity);
	NewtonBodySetTransformCallback(body, DemoEntity::TransformCallback);
	NewtonBodySetForceAndTorqueCallback(body, PhysicsApplyGravityForce);
	NewtonCollision* const collision = NewtonBodyGetCollision(body);

	#ifdef USE_STATIC_MESHES_DEBUG_COLLISION
	if (NewtonCollisionGetType(collision) == SERIALIZE_ID_TREE) {
		NewtonStaticCollisionSetDebugCallback (collision, ShowMeshCollidingFaces);
	}
	#endif

	//for visual mesh we will collision mesh and convert it to a visual mesh using NewtonMesh 
	dTree <DemoMeshInterface*, const void*>* const cache = (dTree <DemoMeshInterface*, const void*>*)bodyUserData;
	dTree <DemoMeshInterface*, const void*>::dTreeNode* node = cache->Find(NewtonCollisionDataPointer (collision));
	if (!node) {
		DemoMeshInterface* mesh = new DemoMesh(bodyIndentification, collision, NULL, NULL, NULL);
		node = cache->Insert(mesh, NewtonCollisionDataPointer (collision));
	} else {
		node->GetInfo()->AddRef();
	}

	DemoMeshInterface* const mesh = node->GetInfo();
	entity->SetMesh(mesh, dGetIdentityMatrix());
	mesh->Release();
}

int DemoEntityManager::Print (const dVector& color, const char *fmt, ... ) const
{
	va_list argptr;
	char string[1024];

	va_start (argptr, fmt);
	vsprintf (string, fmt, argptr);
	va_end( argptr );
	ImGui::Text(string);
	return 0;
}

void DemoEntityManager::SetCameraMatrix (const dQuaternion& rotation, const dVector& position)
{
	m_cameraManager->SetCameraMatrix(this, rotation, position);
}


void DemoEntityManager::UpdatePhysics(dFloat timestep)
{
	// update the physics
	if (m_world && !m_suspendPhysicsUpdate) {

		dFloat timestepInSecunds = 1.0f / MAX_PHYSICS_FPS;
		unsigned64 timestepMicrosecunds = unsigned64 (timestepInSecunds * 1000000.0f);

		unsigned64 currentTime = dGetTimeInMicrosenconds ();
		unsigned64 nextTime = currentTime - m_microsecunds;
		if (nextTime > timestepMicrosecunds * 2) {
			m_microsecunds = currentTime - timestepMicrosecunds * 2;
			nextTime = currentTime - m_microsecunds;
		}

		bool newUpdate = false;
		dFloat physicsTime = 0.0f;
		//while (nextTime >= timestepMicrosecunds) 
		if (nextTime >= timestepMicrosecunds) 
		{
			newUpdate = true;
			ClearDebugDisplay(m_world);

			if (m_asynchronousPhysicsUpdate) {
				NewtonUpdateAsync(m_world, timestepInSecunds);
			} else {
				NewtonUpdate(m_world, timestepInSecunds);
			}
			physicsTime += NewtonGetLastUpdateTime(m_world);
			
			nextTime -= timestepMicrosecunds;
			m_microsecunds += timestepMicrosecunds;
		}

		if (newUpdate) {
			m_physicsFramesCount ++;
			m_mainThreadPhysicsTimeAcc += physicsTime;
			if (m_physicsFramesCount >= 16) {
				m_mainThreadPhysicsTime = m_mainThreadPhysicsTimeAcc / m_physicsFramesCount;
				m_physicsFramesCount = 0;
				m_mainThreadPhysicsTimeAcc = 0.0f;
			}

		}
		
//dTrace (("%f\n", m_mainThreadPhysicsTime));
	}
}

dFloat DemoEntityManager::CalculateInteplationParam () const
{
	unsigned64 timeStep = dGetTimeInMicrosenconds () - m_microsecunds;		
	dFloat param = (dFloat (timeStep) * MAX_PHYSICS_FPS) / 1.0e6f;
	dAssert (param >= 0.0f);
	if (param > 1.0f) {
		param = 1.0f;
	}
	return param;
}


// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void DemoEntityManager::RenderDrawListsCallback(ImDrawData* const draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();

	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

	DemoEntityManager* const window = (DemoEntityManager*)io.UserData;

	ImVec4 clearColor = ImColor(114, 144, 154);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);

	window->RenderScene();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (window->m_renderDemoGUI) {
		window->m_renderDemoGUI(window, window->m_renderUIContext);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Render command lists
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);
	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
	#undef OFFSETOF

	// Restore modified state
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

void DemoEntityManager::PostUpdateCallback(const NewtonWorld* const world, dFloat timestep)
{
	DemoEntityManager* const scene = (DemoEntityManager*) NewtonWorldGetUserData(world);
	scene->m_cameraManager->FixUpdate(scene->GetNewton(), timestep);
	if (scene->m_updateCamera) {
		scene->m_updateCamera(scene, scene->m_updateCameraContext, timestep);
	}
}

void DemoEntityManager::RenderScene()
{
	dFloat timestep = dGetElapsedSeconds();	
	CalculateFPS(timestep);
	UpdatePhysics(timestep);

	// Get the interpolated location of each body in the scene
	m_cameraManager->InterpolateMatrices (this, CalculateInteplationParam());

	ImGuiIO& io = ImGui::GetIO();
	int display_w = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int display_h = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	glViewport(0, 0, display_w, display_h);
	glScissor(0, 0, display_w, display_h);
	glEnable(GL_SCISSOR_TEST);	

	// Rendering
	// Our shading model--Goraud (smooth). 
	glShadeModel (GL_SMOOTH);

	// Culling. 
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);

	//	glEnable(GL_DITHER);
	// z buffer test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// set default lightning
	glEnable (GL_LIGHTING);

	dFloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialParam(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialParam(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// one light form the Camera eye point
	GLfloat lightDiffuse0[] = { 0.5f, 0.5f, 0.5f, 0.0 };
	GLfloat lightAmbient0[] = { 0.0f, 0.0f, 0.0f, 0.0 };
	dVector camPosition (m_cameraManager->GetCamera()->m_matrix.m_posit);
	GLfloat lightPosition0[] = {GLfloat(camPosition.m_x), GLfloat(camPosition.m_y), GLfloat(camPosition.m_z)};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuse0);
	glEnable(GL_LIGHT0);

	// set just one directional light
	GLfloat lightDiffuse1[] = { 0.7f, 0.7f, 0.7f, 0.0 };
	GLfloat lightAmbient1[] = { 0.2f, 0.2f, 0.2f, 0.0 };
	GLfloat lightPosition1[] = { -500.0f, 200.0f, 500.0f, 0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightDiffuse1);
	glEnable(GL_LIGHT1);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

	// Setup matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// make sure the model view matrix is set to identity before setting world space light sources
	// update Camera
	m_cameraManager->GetCamera()->SetViewMatrix(display_w, display_h);

	// render all entities
	if (m_hideVisualMeshes) {
		if (m_sky) {
			glPushMatrix();	
			m_sky->Render(timestep, this);
			glPopMatrix();
		}
	} else {
		for (dListNode* node = dList<DemoEntity*>::GetFirst(); node; node = node->GetNext()) {
			DemoEntity* const entity = node->GetInfo();
			glPushMatrix();	
			entity->Render(timestep, this);
			glPopMatrix();
		}
	}

	DEBUG_DRAW_MODE mode = m_solid;
	if (m_collisionDisplayMode) {
		mode = (m_collisionDisplayMode == 1) ? m_solid : m_lines;
		DebugRenderWorldCollision (m_world, mode);
	}

	if (m_showAABB) {
		RenderAABB (m_world);
	}

	if (m_showContactPoints) {
		RenderContactPoints (m_world);
	}

	if (m_showCenterOfMass) {
		RenderCenterOfMass(m_world);
	}

	if (m_showJointDebugInfo) {
		dJointDebugDisplay jointDebugRender (m_cameraManager->GetCamera()->GetCurrentMatrix());
		RenderJointsDebugInfo(m_world, &jointDebugRender, 0.5f);
	}

	if (m_showNormalForces) {
		RenderNormalForces (m_world);
	}

	if (m_tranparentHeap.GetCount()) {
		glPushMatrix();	
		while (m_tranparentHeap.GetCount()) {
			const TransparentMesh& transparentMesh = m_tranparentHeap[0];
			glLoadIdentity();
			glLoadMatrix(&transparentMesh.m_matrix[0][0]);
			transparentMesh.m_mesh->RenderTransparency();
			m_tranparentHeap.Pop();
		}
		glPopMatrix();
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}



void DemoEntityManager::Run()
{
    // Main loop
    while (!glfwWindowShouldClose(m_mainFrame))
    {
		m_suspendPhysicsUpdate = false;

		BeginFrame();
		RenderStats();

		ImGui::Render();
		glfwSwapBuffers(m_mainFrame);
    }
}