#include <iostream>
#include <PxPhysicsAPI.h> //PhysX main header file
#include <GL/freeglut.h> // GLUT for rendering
#include <PxMat33.h> 


//-------Loading PhysX libraries----------]
#ifdef _DEBUG
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#else
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#endif

using namespace std;
using namespace physx;

// define window dimensions
const int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768;

// create pointer to SDK
static PxPhysics* gPhysicsSDK = NULL;

// create pointer to foundation
static PxFoundation* gFoundation = NULL;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;

// Pointer to scene
PxScene* gScene = NULL;

// define time step
PxReal myTimestep = 1.0f / 60.0f;

// pointer to box object
PxRigidActor *box;

//for mouse dragging
int oldX = 0, oldY = 0;
float rX = 15, rY = 0;
float fps = 0;
int startTime = 0;
int totalFrames = 0;
int state = 1;
float dist = -5;

// Initilialize PhysX
void InitializePhysX() 
{
	//Creating foundation for PhysX
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,	gDefaultErrorCallback);
	//Creating SDK
	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

	if (gPhysicsSDK == NULL) {
		cerr << "Error creating PhysX device." << endl;
		cerr << "Exiting..." << endl;
		exit(1);
	}

	if (!PxInitExtensions(*gPhysicsSDK))
		cerr << "PxInitExtensions failed!" << endl;


	//Create the scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	// define gravity
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	if (!sceneDesc.cpuDispatcher) {
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
			cerr << "PxDefaultCpuDispatcherCreate failed!" << endl;
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = gDefaultFilterShader;

	// create scene
	gScene = gPhysicsSDK->createScene(sceneDesc);

	if (!gScene)
		cerr << "createScene failed!" << endl;

	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	// create material
	PxMaterial* mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	//Create actors 
	//1) Create ground plane
	PxReal d = 0.0f;
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

	// create plane
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(pose);
	if (!plane)
		cerr << "create plane failed!" << endl;

	// assign shape to plane
	PxShape* shape = plane->createShape(PxPlaneGeometry(), *mMaterial);
	if (!shape)
		cerr << "create shape failed!" << endl;
	gScene->addActor(*plane);


	//2) Create cube	 
	PxReal density = 1.0f;
	PxTransform transform(PxVec3(0.0f, 10.0f, 0.0f), PxQuat::createIdentity());
	PxVec3 dimensions(0.5, 0.5, 0.5);
	PxBoxGeometry geometry(dimensions);

	PxRigidDynamic *actor = PxCreateDynamic(*gPhysicsSDK, transform, geometry, *mMaterial, density);
	actor->setAngularDamping(0.75);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor)
		cerr << "create actor failed!" << endl;
	gScene->addActor(*actor);

	// assign actor to global variable
	box = actor;
}

// simulating a tipe step for PhysX, asynchronously
void StepPhysX()
{
	gScene->simulate(myTimestep);

	//...perform useful work here using previous frame's state data        
	while (!gScene->fetchResults())
	{
		// do something useful        
	}
}

// Delete PhysX objects
void ShutdownPhysX()
{
	gPhysicsSDK->release();
}

// Draw the shape
void DrawShape(PxShape* shape)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
		DrawBox(shape);
		break;
	}
}


// Drawing the actor by getting its shape
void DrawActor(PxRigidActor* actor)
{
	PxU32 nShapes = actor->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	actor->getShapes(shapes, nShapes);
	while (nShapes--)
	{
		DrawShape(shapes[nShapes]);
	}
	delete[] shapes;
}

// Rendering all actors for scene
void RenderActors()
{
	// Render all the actors in the scene 

	DrawActor(box);
}

// Drawp a box in GLUT
void DrawBox(PxShape* pShape)
{
	PxTransform pT = PxShapeExt::getGlobalPose(*pShape);
	PxBoxGeometry bg;
	pShape->getBoxGeometry(bg);
	PxMat33 m = PxMat33(pT.q);
	float mat[16];
	getColumnMajor(m, pT.p, mat);
	glPushMatrix();
	glMultMatrixf(mat);
	glutSolidCube(bg.halfExtents.x * 2);
	glPopMatrix();
}

// Rendering function for GLUT
void OnRender()
{
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//reset matrix
	glLoadIdentity();

	glutSwapBuffers();
}

// For window respahing
void OnReshape(int nw, int nh)
{
	// viewport set to entire window
	glViewport(0, 0, nw, nh);

	// projection matrix
	glMatrixMode(GL_PROJECTION);

	// reset matrix
	glLoadIdentity();

	// set correct perspective
	gluPerspective(60, (GLfloat)nw / (GLfloat)nh, 0.1f, 100.0f);

	// Return to model view
	glMatrixMode(GL_MODELVIEW);
}

// support function for getting global position
void getColumnMajor(PxMat33 m, PxVec3 t, float* mat) {
	mat[0] = m.column0[0];
	mat[1] = m.column0[1];
	mat[2] = m.column0[2];
	mat[3] = 0;

	mat[4] = m.column1[0];
	mat[5] = m.column1[1];
	mat[6] = m.column1[2];
	mat[7] = 0;

	mat[8] = m.column2[0];
	mat[9] = m.column2[1];
	mat[10] = m.column2[2];
	mat[11] = 0;

	mat[12] = t[0];
	mat[13] = t[1];
	mat[14] = t[2];
	mat[15] = 1;
}

int main(int argc, char **argv) {
	// C++ own at-exit function
	atexit(ShutdownPhysX);

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("GLUT PhysX3 BouncingBox");

	// register callbacks
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);

	// enter GLUT event processing cycle
	glutMainLoop();

}