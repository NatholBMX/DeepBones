#include <iostream>
#include <PxPhysicsAPI.h> //PhysX main header file

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

// parameters for foundation
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* gFoundation = NULL;
static PxPhysics* gPhysicsSDK = NULL;
PxRigidDynamic* gBox;

PxScene* gScene = NULL;
void InitPhysX()
{
	//Creating foundation for PhysX
	gFoundation = PxCreateFoundation
		(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);

	
	//Creating instance of PhysX SDK
	gPhysicsSDK = PxCreatePhysics
		(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

	if (gPhysicsSDK == NULL)
	{
		cerr << "Error creating PhysX3 device, Exiting..." << endl;
		exit(1);
	}


	//Creating scene
	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());

	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysicsSDK->createScene(sceneDesc);

	//Creating material
	PxMaterial* mMaterial =
		//static friction, dynamic friction, restitution
		gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	//1-Creating static plane
	// place plane at originin, rotate plane by 90 degrees in z-axis
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = gPhysicsSDK->createRigidStatic(planePos);
	// create plane, add material
	plane->createShape(PxPlaneGeometry(), *mMaterial);
	gScene->addActor(*plane);

	
	// Create cube
	// place it 10 units above origin
	PxTransform boxPos(PxVec3(0.0f, 10.0f, 0.0f));
	PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
	gBox = PxCreateDynamic(*gPhysicsSDK, boxPos, boxGeometry, *mMaterial, 1.0f);
	gScene->addActor(*gBox);

}



//Stepping PhysX
PxReal myTimestep = 1.0f / 60.0f;

void StepPhysX()
{
	gScene->simulate(myTimestep);
	gScene->fetchResults(true);
}



void ShutdownPhysX()
{
	gScene->release();
	gPhysicsSDK->release();
	gFoundation->release();
}

void main(int argc, char** argv)
{
	InitPhysX();

	//Simulate PhysX 300 times
	for (int i = 0; i <= 300; i++)
	{
		//Step PhysX simulation
		if (gScene)
			StepPhysX();
		//Get current position of actor (box) and print it
		PxVec3 boxPos = gBox->getGlobalPose().p;
		cout << "Box current Position (" << boxPos.x << " " << boxPos.y << " " << boxPos.z << ")\n";
	}

	char in;
	cin >> in;

}