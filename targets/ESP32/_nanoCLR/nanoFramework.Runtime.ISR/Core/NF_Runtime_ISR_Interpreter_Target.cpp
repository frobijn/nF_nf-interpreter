//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"

void *NF_RunTime_ISR_AllocateMemory (InterpreterMemoryType memoryType, NF_Runtime_ISR_SharedDataOffsetType size)
{
    return nullptr;
}

void NF_RunTime_ISR_ReleaseMemory (InterpreterMemoryType memoryType, void *memory)
{

}

static void RTOSTask (NF_Runtime_ISR_InterruptHandler *interruptHandler)
{
    NF_RunTime_ISR_RunFromRTOSTask(interruptHandler);
}

void NF_RunTime_ISR_QueueRTOSTask (NF_Runtime_ISR_InterruptHandler *interruptHandler)
{
    RTOSTask(interruptHandler);
}

void NF_RunTime_ISR_RunServiceRoutineInSeparateRTOSTask (NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    
}



