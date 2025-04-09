﻿//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RUNTIME_ISR_H
#define NF_RUNTIME_ISR_H

//======================================================================
//
// This header for the ISR interpreter is used only in the
// nf-interpreter / CLR project project. There is an equivalent
// header in the nanoFramework.Runtime.ISR.Interpreter project.
// 
//======================================================================

#include <nanoCLR_Interop.h>

#define NF_RUNTIME_ISR_FOR_NANOCLR

#include "nf_runtime_isr_core.h"
#include "NF_Runtime_ISR_ByteCode.h"
#include "NF_Runtime_ISR_Interpreter.h"

#define ARG_AS_HEAPOFFSETTYPE(arg)              arg.NumericByRef().u2
#define ARG_AS_SHAREDDATAOFFSETTYPE(arg)        arg.NumericByRef().u4
#define ARG_AS_INTPTR(arg)                      (void *)(arg.NumericByRef().s4)
#define SET_RESULT_AS_SHAREDDATAOFFSETTYPE(x)   stack.SetResult_U4(x)
#define SET_RESULT_AS_INTPTR(x)                 stack.SetResult_I4((CLR_INT32)x)

#endif // !NF_RUNTIME_ISR_H
