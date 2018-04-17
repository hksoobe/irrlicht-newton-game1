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


// NewtonCustomJoint.cpp: implementation of the NewtonCustomJoint class.
//
//////////////////////////////////////////////////////////////////////
#include "dCustomJointLibraryStdAfx.h"
#include "dCustomJoint.h"
#include "dCustomModelLoadSave.h"

dCRCTYPE dCustomJoint::m_key = dCRC64 ("dCustomJoint");
dCustomJoint::dSerializeMetaData m_metaData_CustomJoint("dCustomJoint");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
dCustomJoint::dCustomJoint ()
	:dCustomAlloc()
	,m_localMatrix0(dGetIdentityMatrix())
	,m_localMatrix1(dGetIdentityMatrix())
	,m_userData(NULL)
	,m_body0(NULL)
	,m_body1(NULL)
	,m_joint(NULL)
	,m_world(NULL)
	,m_userDestructor(NULL)
	,m_stiffness(1.0f)
	,m_maxDof(0)
	,m_autoDestroy(0)
{
}

dCustomJoint::dCustomJoint (int maxDOF, NewtonBody* const body0, NewtonBody* const body1)
	:dCustomAlloc()
{
	Init (maxDOF, body0, body1);
}

dCustomJoint::dCustomJoint(NewtonInverseDynamics* const invDynSolver, void* const invDynNode)
	:dCustomAlloc()
	,m_localMatrix0(dGetIdentityMatrix())
	,m_localMatrix1(dGetIdentityMatrix())
{
	m_joint = NULL;
	m_body1 = NULL;
	m_maxDof = 6;
	m_autoDestroy = 0;
	m_stiffness = 1.0f;
	m_body0 = NewtonInverseDynamicsGetBody (invDynSolver, invDynNode);
	m_world = NewtonBodyGetWorld(m_body0);
	m_joint = NewtonInverseDynamicsCreateEffector(invDynSolver, invDynNode, SubmitConstraints);

	NewtonJointSetUserData(m_joint, this);
	NewtonJointSetDestructor(m_joint, Destructor);

	m_userData = NULL;
	m_userDestructor = NULL;
}

dCustomJoint::dCustomJoint (NewtonBody* const body0, NewtonBody* const body1, NewtonDeserializeCallback callback, void* const userData)
	:dCustomAlloc()
	,m_localMatrix0(dGetIdentityMatrix())
	,m_localMatrix1(dGetIdentityMatrix())
	,m_userData(NULL)
	,m_body0(body0)
	,m_body1(body1)
	,m_joint(NULL)
	,m_world(NULL)
	,m_userDestructor(NULL)
	,m_stiffness(1.0f)
	,m_maxDof(0)
	,m_autoDestroy(0)
{
	int solverModel;
	callback (userData, &m_localMatrix0, sizeof (m_localMatrix0));
	callback (userData, &m_localMatrix1, sizeof (m_localMatrix1));
	callback (userData, &m_maxDof, sizeof (m_maxDof));
	callback (userData, &m_stiffness, sizeof (m_stiffness));
	callback(userData, &solverModel, sizeof(solverModel));

	Init (m_maxDof, body0, body1);
	SetSolverModel(solverModel);
}


dCustomJoint::dCustomJoint(dCustomJointSaveLoad* const fileLoader, NewtonBody* const body0, NewtonBody* const body1)
	:dCustomAlloc()
	,m_localMatrix0(dGetIdentityMatrix())
	,m_localMatrix1(dGetIdentityMatrix())
	,m_userData(NULL)
	,m_body0(body0)
	,m_body1(body1)
	,m_joint(NULL)
	,m_world(NULL)
	,m_userDestructor(NULL)
	,m_stiffness(1.0f)
	,m_maxDof(0)
	,m_autoDestroy(0)
{
	const char* token = fileLoader->NextToken();
	dAssert(!strcmp(token, "degreesOfFreedom:"));
	m_maxDof = fileLoader->LoadInt();

	token = fileLoader->NextToken();
	dAssert(!strcmp(token, "solverModel:"));
	int solverModel = fileLoader->LoadInt();

	token = fileLoader->NextToken();
	dAssert(!strcmp(token, "localMatrix0:"));
	m_localMatrix0 = fileLoader->LoadMatrix();
	
	token = fileLoader->NextToken();
	dAssert(!strcmp(token, "localMatrix1:"));
	m_localMatrix1 = fileLoader->LoadMatrix();

	token = fileLoader->NextToken();
	dAssert(!strcmp(token, "stiffness:"));
	m_stiffness = fileLoader->LoadFloat();

	Init(m_maxDof, body0, body1);
	SetSolverModel(solverModel);
}


dCustomJoint::~dCustomJoint()
{
	//dAssert (m_joint);
	
	//if the joint has user data it means the application is destroying the joint

// if there is a C destructor call it form here
	dCustomJoint* const joint = (dCustomJoint*) NewtonJointGetUserData (m_joint);  
	if (joint->m_userDestructor) {
		//joint->m_userDestructor ((const NewtonUserJoint*) joint);
		joint->m_userDestructor (joint);
	}

//	if (NewtonJointGetUserData (m_joint)) {
	if (!m_autoDestroy) {
		// set the joint call to NULL to prevent infinite recursion 
		NewtonJointSetUserData (m_joint, NULL);  
		NewtonJointSetDestructor (m_joint, NULL);  

		// destroy this joint
		NewtonDestroyJoint(m_world, m_joint);
	}
}

void dCustomJoint::Initalize(NewtonWorld* const world)
{
	NewtonSetJointSerializationCallbacks (world, Serialize, Deserialize);
}

void dCustomJoint::Init (int maxDOF, NewtonBody* const body0, NewtonBody* const body1)
{
	m_joint = NULL;
	m_body0 = body0;
	m_body1 = body1;
	m_maxDof = maxDOF;
	m_autoDestroy = 0;
	m_stiffness = 1.0f;

	m_world	= body0 ? NewtonBodyGetWorld (body0) : NewtonBodyGetWorld (body1);
	m_joint = NewtonConstraintCreateUserJoint (m_world, maxDOF, SubmitConstraints, m_body0, m_body1); 

	NewtonJointSetUserData (m_joint, this);
	NewtonJointSetDestructor (m_joint, Destructor);

	m_userData = NULL;
	m_userDestructor = NULL;
}


const dMatrix& dCustomJoint::GetMatrix0() const
{
	return m_localMatrix0;
}

const dMatrix& dCustomJoint::GetMatrix1() const
{
	return m_localMatrix1;
}


NewtonBody* dCustomJoint::GetBody0 () const
{
	return m_body0;
}

NewtonBody* dCustomJoint::GetBody1 () const
{
	return m_body1;
}

NewtonJoint* dCustomJoint::GetJoint () const
{
	return m_joint;
}

dFloat dCustomJoint::GetStiffness() const
{
	return m_stiffness;
}

void dCustomJoint::SetStiffness(dFloat stiffness)
{
	m_stiffness = dClamp (stiffness, dFloat(0.0f), dFloat(1.0f));
}

void dCustomJoint::SetSolverModel(int model)
{
	NewtonUserJointSetSolverModel (m_joint, model);
}

int dCustomJoint::GetSolverModel() const
{
	return NewtonUserJointGetSolverModel(m_joint);
}


void dCustomJoint::Destructor (const NewtonJoint* me)
{
	// get the pointer to the joint class
	dCustomJoint* const joint = (dCustomJoint*) NewtonJointGetUserData (me);  

	joint->m_autoDestroy = 1;

	// delete the joint class
	delete joint;
}

void dCustomJoint::Debug(dDebugDisplay* const debugDisplay) const
{
	dMatrix matrix0;
	dMatrix matrix1;
	CalculateGlobalMatrix(matrix0, matrix1);
	debugDisplay->DrawFrame(matrix0);
	debugDisplay->DrawFrame(matrix1);
}

void dCustomJoint::SubmitConstraints (const NewtonJoint* const me, dFloat timestep, int threadIndex)
{
	// get the pointer to the joint class
	if (timestep != 0.0f) {
		dCustomJoint* const joint = (dCustomJoint*) NewtonJointGetUserData (me);  

		// call the constraint call back
		if (joint) {
			joint->SubmitConstraints(timestep, threadIndex);
		}
	}
}


void dCustomJoint::Serialize (const NewtonJoint* const me, NewtonSerializeCallback callback, void* const userData)
{
	dCustomJoint* const joint = (dCustomJoint*) NewtonJointGetUserData (me);  

	dCRCTYPE key = joint->GetSerializeKey();
	callback (userData, &key, sizeof (key));

	const dSerializeMetaDataDictionary& dictionary = GetDictionary();
	dSerializeMetaDataDictionary::dTreeNode* const node = dictionary.Find(key); 
	if (node) {
		dSerializeMetaData* const meta = node->GetInfo();
		meta->SerializeJoint(joint, callback, userData);
	}
}

void dCustomJoint::Deserialize (NewtonBody* const body0, NewtonBody* const body1, NewtonDeserializeCallback callback, void* const userData)
{
	dCRCTYPE key;
	callback (userData, &key, sizeof (key));
	const dSerializeMetaDataDictionary& dictionary = GetDictionary();

	dSerializeMetaDataDictionary::dTreeNode* const node = dictionary.Find(key); 
	if (node) {
		dSerializeMetaData* const meta = node->GetInfo();
		meta->DeserializeJoint (body0, body1, callback, userData);
	}
}

dCustomJoint* dCustomJoint::Load(dCustomJointSaveLoad* const fileLoader, const char* const jointType, NewtonBody* const body0, NewtonBody* const body1)
{
	dCRCTYPE key (dCRC64 (jointType));
	const dSerializeMetaDataDictionary& dictionary = GetDictionary();
	dCustomJoint* joint = NULL;
	dSerializeMetaDataDictionary::dTreeNode* const node = dictionary.Find(key); 
	dAssert (node);
	if (node) {
		dSerializeMetaData* const meta = node->GetInfo();
		joint = meta->Load(fileLoader, body0, body1);
	}

	return joint;
}


void dCustomJoint::CalculateLocalMatrix (const dMatrix& pinsAndPivotFrame, dMatrix& localMatrix0, dMatrix& localMatrix1) const
{
	dMatrix matrix0;

	// Get the global matrices of each rigid body.
	NewtonBodyGetMatrix(m_body0, &matrix0[0][0]);
	dMatrix matrix1 (dGetIdentityMatrix());
	if (m_body1) {
		NewtonBodyGetMatrix(m_body1, &matrix1[0][0]);
	}

	// create a global matrix at the pivot point with front vector aligned to the pin vector
	dAssert (pinsAndPivotFrame.SanityCheck());
	// calculate the relative matrix of the pin and pivot on each body
 	localMatrix0 = pinsAndPivotFrame * matrix0.Inverse();
	localMatrix1 = pinsAndPivotFrame * matrix1.Inverse();
}


void dCustomJoint::CalculateGlobalMatrix (dMatrix& matrix0, dMatrix& matrix1) const
{
	dMatrix body0Matrix;
	// Get the global matrices of each rigid body.
	NewtonBodyGetMatrix(m_body0, &body0Matrix[0][0]);

	dMatrix body1Matrix (dGetIdentityMatrix());
	if (m_body1) {
		NewtonBodyGetMatrix(m_body1, &body1Matrix[0][0]);
	}
	matrix0 = m_localMatrix0 * body0Matrix;
	matrix1 = m_localMatrix1 * body1Matrix;
}

dFloat dCustomJoint::CalculateAngle (const dVector& dir, const dVector& cosDir, const dVector& sinDir, dFloat& sinAngle, dFloat& cosAngle) const
{
	cosAngle = dir.DotProduct3(cosDir);
	sinAngle = sinDir.DotProduct3(dir.CrossProduct(cosDir));
	return dAtan2(sinAngle, cosAngle);
}

dFloat dCustomJoint::CalculateAngle (const dVector& dir, const dVector& cosDir, const dVector& sinDir) const
{
	dFloat sinAngle;
	dFloat cosAngle;
	return CalculateAngle (dir, cosDir, sinDir, sinAngle, cosAngle);
}


void dCustomJoint::SetBodiesCollisionState (int state)
{
	NewtonJointSetCollisionState (m_joint, state);
}

int dCustomJoint::GetBodiesCollisionState () const
{
	return NewtonJointGetCollisionState (m_joint);
}

void dCustomJoint::SubmitConstraints (dFloat timestep, int threadIndex)
{
}



const char* dCustomJoint::GetTypeName() const
{
	return "dCustomJoint";
}

bool dCustomJoint::IsType (dCRCTYPE type) const
{
	return false;
}

dCRCTYPE dCustomJoint::GetSerializeKey() const
{
	return m_key;
}


dCustomJoint::dSerializeMetaData::dSerializeMetaData(const char* const name)
{
	dCustomJoint::GetDictionary().Insert(this, dCRC64(name));
}


void dCustomJoint::dSerializeMetaData::SerializeJoint (dCustomJoint* const joint, NewtonSerializeCallback callback, void* const userData)
{
	dAssert (0);
}

dCustomJoint* dCustomJoint::dSerializeMetaData::DeserializeJoint (NewtonBody* const body0, NewtonBody* const body1, NewtonDeserializeCallback callback, void* const userData)
{
	dAssert (0);
	return NULL;
}

dCustomJoint* dCustomJoint::dSerializeMetaData::Load (dCustomJointSaveLoad* const fileLoader, NewtonBody* const body0, NewtonBody* const body1)
{
	dAssert(0);
	return NULL;
}

dCustomJoint::dSerializeMetaDataDictionary& dCustomJoint::GetDictionary()
{
	static dSerializeMetaDataDictionary dictionary;
	return dictionary;
}


void dCustomJoint::Serialize (NewtonSerializeCallback callback, void* const userData) const
{
	int solverModel = GetSolverModel();
	callback (userData, &m_localMatrix0, sizeof (m_localMatrix0));
	callback (userData, &m_localMatrix1, sizeof (m_localMatrix1));
	callback (userData, &m_maxDof, sizeof (m_maxDof));
	callback (userData, &m_stiffness, sizeof (m_stiffness));
	callback(userData, &solverModel, sizeof(solverModel));
}

void dCustomJoint::Deserialize (NewtonSerializeCallback callback, void* const userData) const
{
	dAssert (0);
}

void dCustomJoint::dDebugDisplay::DrawFrame(const dMatrix& matrix)
{
	dVector o0(matrix.m_posit);

	dFloat size = 0.25f;
	dVector x(matrix.m_posit + matrix.RotateVector(dVector(size, 0.0f, 0.0f, 0.0f)));
	SetColor(dVector (1.0f, 0.0f, 0.0f));
	DrawLine (matrix.m_posit, x);

	dVector y(matrix.m_posit + matrix.RotateVector(dVector(0.0f, size, 0.0f, 0.0f)));
	SetColor(dVector (0.0f, 1.0f, 0.0f));
	DrawLine (matrix.m_posit, y);

	dVector z(matrix.m_posit + matrix.RotateVector(dVector(0.0f, 0.0f, size, 0.0f)));
	SetColor(dVector (0.0f, 0.0f, 1.0f));
	DrawLine (matrix.m_posit, z);
}


void dCustomJoint::Save(dCustomJointSaveLoad* const fileSaver) const
{
	fileSaver->SaveInt ("\tdegreesOfFreedom", m_maxDof);
	fileSaver->SaveInt ("\tsolverModel", GetSolverModel());

	fileSaver->SaveMatrix ("\tlocalMatrix0", m_localMatrix0);
	fileSaver->SaveMatrix ("\tlocalMatrix1", m_localMatrix1);
	fileSaver->SaveFloat("\tstiffness", m_stiffness);
}

void dCustomJoint::Load(dCustomJointSaveLoad* const loader)
{
	dAssert (0);
}