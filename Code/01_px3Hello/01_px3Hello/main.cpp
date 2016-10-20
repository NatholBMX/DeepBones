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

//Creating foundation for PhysX
gFoundation = PxCreateFoundation
(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
gDefaultErrorCallback);

static PxPhysics* gPhysicsSDK = NULL;
//Creating instance of PhysX SDK
gPhysicsSDK = PxCreatePhysics
(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
if (gPhysicsSDK == NULL)
{
	cerr << "Error creating PhysX3 device, Exiting..." << endl;
	exit(1);
}