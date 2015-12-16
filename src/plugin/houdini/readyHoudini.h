/*
 * Copyright (c) 2013
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * Based on: CPPWave SOP
 * Changes by Dan Wills for 'Ready' reaction-diffusion plugin.
 */


#ifndef __readyHoudini_h__
#define __readyHoudini_h__


// STL:
#include <iostream>
using namespace std;

// stdlib:
#include <stdlib.h>
#include <ctype.h>
#include <float.h>

// Houdini:
#include <SOP/SOP_Node.h>
//#include <opencl.h>
//#include <cl.hpp>
//#include <CE/cl.hpp>
//#include <CE/CE_Error.h>
//#include <CE/CE_Grid.h>
//#include <CE/CE_MemoryPool.h>
//#include <CE/CE_Multigrid.h>
//#include <CE/CE_SparseMatrix.h>
//#include <CE/CE_Vector.h>
//#include <CE/CE_API.h>
#include <CE/CE_Context.h>

#define __EXTERNAL_OPENCL__
// readybase:
#include <SystemFactory.hpp>
#include <Properties.hpp>
#include <OpenCL_utils.hpp>
#include <OpenCL_MixIn.hpp>
#include <AbstractRD.hpp>

// vtk
#include <vtkSmartPointer.h>
#include <vtkImageData.h>


#define INT_PARM(name, idx, vidx, t)	\
	    return evalInt(name, &indexOffsets[idx], vidx, t);
#define FLOAT_PARM(name, idx, vidx, t)	\
	    return evalFloat(name, &indexOffsets[idx], vidx, t);
		
class readyHoudini : public SOP_Node
{
public:
	readyHoudini(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~readyHoudini();

	static PRM_Template          myTemplateList[];
    //static PRM_Template		 getTemplateList();
    static OP_Node		*myConstructor(OP_Network*, const char *,
							    OP_Operator *);
	void initRenderProperties( Properties &render_settings );
	void updateVtiFile( const char *update_file );
	void updateSystemFromParms( OP_Context &context );
	void addSpareParms(const PRM_Template *spareParmTemplate, const char *folderName);
	float float_parm( const char *name, int idx, int vidx, fpreal t );
	bool parmChanged( OP_Context &context );
	void clearSpareParms();
	bool parmNameInTemplateList( PRM_Name parm, PRM_Template* prmList, int listLength );
	bool parmTemplateInNameList( PRM_Template parm, PRM_Name* prmNameList, int listLength );
	bool strInPrmNameList( std::string parmName, PRM_Name* prmNameList, int listLength );
	bool parmNameInPrmNameList( PRM_Name parmName, PRM_Name* prmNameList, int listLength );
	void clearSpareParmsNotInList( PRM_Name *oldParmNames, int numOldParms, PRM_Name* newParmNames, int numNewParms );
	void updateCopyBuffersAndMap( int rReagent, int bReagent, int cReagent, int xres, int yres, int zres );
	void updateCopyBuffersIfNeeded( bool force );
	int getReagentOffset( int baseReagentIndex );

protected:
    virtual OP_ERROR		 cookMySop(OP_Context &context);
private:
	Properties *render_settings;
	AbstractRD *system;
	UT_String *loadedVtiName;
	UT_String *writeAttributeName;
	float start_frame;
	float last_cooked_frame;
	int steps_per_frame;
	int step_count;
	
	int system_resx;
	int system_resy;
	int system_resz;
	
	int X_RES() { INT_PARM("xRes", 0, 0, 0) }
	int Y_RES() { INT_PARM("yRes", 0, 0, 0) }
	int Z_RES() { INT_PARM("zRes", 0, 0, 0) }
	int REGEN() { INT_PARM("regenerateInitialState", 0, 0, 0) }
	
	bool is_opencl_available;
	int opencl_platform; // TODO: get from houdini context
	int opencl_device; // TODO: get from houdini context
	bool warn_to_update;
	
	cl_context raw_context;
	float *rd_data;
	unsigned long reagent_block_size;
	int num_reagents;
	int num_parameters;
	PRM_Name *parmNames;
	float *parmValues;
	int old_num_parameters;
	PRM_Name *oldParmNames;
	int *reagentCopyMap;
	int numCopyReagents;
	static int *indexOffsets;
	
	int REAGENT_R() { INT_PARM("reagentR", 0, 0, 0) }
	int REAGENT_G() { INT_PARM("reagentG", 0, 0, 0) }
	int REAGENT_B() { INT_PARM("reagentB", 0, 0, 0) }
	
	// int APPLY_MASKS() { INT_PARM("applyMasks", 0, 0, 0) } //applyMasks
	// float MASKMULTR() { FLOAT_PARM("maskMultR", 0, 0, 0) } //maskMultR
	// float MASKMULTG() { FLOAT_PARM("maskMultG", 0, 0, 0) } //maskMultG
	// float MASKMULTB() { FLOAT_PARM("maskMultB", 0, 0, 0) } //maskMultB
	// float MASKADDR() { FLOAT_PARM("maskAddR", 0, 0, 0)} //maskAddR
	// float MASKADDG() { FLOAT_PARM("maskAddG", 0, 0, 0) } //maskAddG
	// float MASKADDB() { FLOAT_PARM("maskAddB", 0, 0, 0) } //maskAddB
	
	int reagentR;
	int reagentG;
	int reagentB;
	
	int START_FRAME() { INT_PARM("startFrame", 0, 0, 0) }
	int STEPS_PER_FRAME() { INT_PARM("stepsPerFrame", 0, 0, 0) }
	void VTIFILE(UT_String &path, fpreal t) { evalString(path, "vtiFile", 0, t); }
	void WRITEATTRIBNAME(UT_String &path, fpreal t) { evalString(path, "writeAttribute", 0, t); }
};

#endif

