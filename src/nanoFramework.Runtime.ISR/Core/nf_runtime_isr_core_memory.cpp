//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "nf_runtime_isr_core_memory.h"

//----------------------------------------------------------------------
//
// Read/write managed data from/to memory
//
//----------------------------------------------------------------------

HRESULT ReadManagedData(CLR_RT_HeapBlock *instanceArg, CLR_UINT8 **dataType, bool asArrayElement, CLR_UINT8 **data)
{
    NANOCLR_HEADER();

    if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0 && !asArrayElement)
    {
        CLR_UINT8 elementType = ~NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY & **dataType;
        CLR_RT_HeapBlock_Array *array = instanceArg->DereferenceArray();
        if (array == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
        }

        if ((elementType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
        {
            CLR_UINT8 *elementPtr = array->GetFirstElement();
            CLR_UINT8 *elementDataType = *dataType;
            for (CLR_UINT32 iElement = 0; iElement < array->m_numOfElements;
                 iElement++, elementPtr += array->m_sizeOfElement)
            {
                *dataType = elementDataType;
                NANOCLR_CHECK_HRESULT(ReadManagedData((CLR_RT_HeapBlock *)elementPtr, dataType, true, data));
            }
        }
        else
        {
            if (array->m_typeOfElement != elementType)
            {
#ifdef ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS
                ISR_Diagnostics_IncorrectArrayType(array->m_typeOfElement, elementType);
#endif
                NANOCLR_MSG_SET_AND_LEAVE(CLR_E_WRONG_TYPE, msg);
            }

            CLR_UINT32 numBytes = array->m_numOfElements * array->m_sizeOfElement;
            memcpy(array->GetFirstElement(), *data, numBytes);
            *data += numBytes;
            (*dataType)++;
        }
    }
    else if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an object reference, the referenced heap block (instanceWithFields)
        // is the struct or class instance it references (and that has the data).
        CLR_RT_HeapBlock *instanceWithFields = instanceArg;
        if (instanceArg->DataType() == CLR_DataType::DATATYPE_OBJECT)
        {
            instanceWithFields = instanceArg->Dereference();
        }

        if (instanceWithFields->DataType() != CLR_DataType::DATATYPE_VALUETYPE &&
            instanceWithFields->DataType() != CLR_DataType::DATATYPE_CLASS)
        {
#ifdef ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS
            ISR_Diagnostics_IncorrectObjectType(instanceWithFields->DataType());
#endif
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        CLR_UINT8 numFields =
            ~(NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT | NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) & *(*dataType)++;
        for (CLR_UINT8 iField = 0; iField < numFields; iField++)
        {
            CLR_UINT8 fieldIndex = *(*dataType)++;
            NANOCLR_CHECK_HRESULT(ReadManagedData(&instanceWithFields[fieldIndex], dataType, false, data));
        }
    }
    else
    {
        switch (*(NF_Runtime_ISR_CompiledDataType *)(*dataType)++)
        {
            case NF_Runtime_ISR_CompiledDataType::Boolean:
                instanceArg->SetBoolean(*(*data)++ != NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_FALSE);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt8:
                instanceArg->SetInteger(*(*data)++);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int8:
                instanceArg->SetInteger(*(CLR_INT8 *)(*data)++);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt16:
                instanceArg->SetInteger(*(CLR_UINT16 *)*data);
                *data += sizeof(CLR_UINT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int16:
                instanceArg->SetInteger(*(CLR_INT16 *)*data);
                *data += sizeof(CLR_INT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt32:
                instanceArg->SetInteger(*(CLR_UINT32 *)*data);
                *data += sizeof(CLR_UINT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int32:
                instanceArg->SetInteger(*(CLR_INT32 *)*data);
                *data += sizeof(CLR_INT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt64:
                instanceArg->SetInteger(*(CLR_UINT64 *)*data);
                *data += sizeof(CLR_UINT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int64:
                instanceArg->SetInteger(*(CLR_INT64 *)*data);
                *data += sizeof(CLR_INT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Single:
                instanceArg->SetFloat(*(float *)*data);
                *data += sizeof(float);
                break;

            case NF_Runtime_ISR_CompiledDataType::Double:
                instanceArg->SetDouble(*(double *)*data);
                *data += sizeof(double);
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_UNSUPPORTED_INSTRUCTION);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT WriteManagedData(CLR_RT_HeapBlock *instanceArg, CLR_UINT8 **dataType, bool asArrayElement, CLR_UINT8 **data)
{
    NANOCLR_HEADER();

    if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0 && !asArrayElement)
    {
        CLR_UINT8 elementType = ~NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY & **dataType;
        CLR_RT_HeapBlock_Array *array = instanceArg->DereferenceArray();
        if (array == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
        }

        if ((elementType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
        {
            CLR_UINT8 *elementPtr = array->GetFirstElement();
            CLR_UINT8 *elementDataType = *dataType;
            for (CLR_UINT32 iElement = 0; iElement < array->m_numOfElements;
                 iElement++, elementPtr += array->m_sizeOfElement)
            {
                *dataType = elementDataType;
                NANOCLR_CHECK_HRESULT(WriteManagedData((CLR_RT_HeapBlock *)elementPtr, dataType, true, data));
            }
        }
        else
        {
            if (array->m_typeOfElement != elementType)
            {
#ifdef ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS
                ISR_Diagnostics_IncorrectArrayType(array->m_typeOfElement, elementType);
#endif
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            CLR_UINT32 numBytes = array->m_numOfElements * array->m_sizeOfElement;
            memcpy(*data, array->GetFirstElement(), numBytes);
            *data += numBytes;
            (*dataType)++;
        }
    }
    else if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an object reference, the referenced heap block (instanceWithFields)
        // is the struct or class instance it references (and that has the data).
        CLR_RT_HeapBlock *instanceWithFields = instanceArg;
        if (instanceArg->DataType() == CLR_DataType::DATATYPE_OBJECT)
        {
            instanceWithFields = instanceArg->Dereference();
        }

        if (instanceWithFields->DataType() != CLR_DataType::DATATYPE_VALUETYPE &&
            instanceWithFields->DataType() != CLR_DataType::DATATYPE_CLASS)
        {
#ifdef ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS
            ISR_Diagnostics_IncorrectObjectType(instanceWithFields->DataType());
#endif
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        CLR_UINT8 numFields =
            ~(NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT | NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) & *(*dataType)++;
        for (CLR_UINT8 iField = 0; iField < numFields; iField++)
        {
            CLR_UINT8 fieldIndex = *(*dataType)++;
            NANOCLR_CHECK_HRESULT(WriteManagedData(&instanceWithFields[fieldIndex], dataType, false, data));
        }
    }
    else
    {
        switch (*(NF_Runtime_ISR_CompiledDataType *)(*dataType)++)
        {
            case NF_Runtime_ISR_CompiledDataType::Boolean:
                *(*data)++ = instanceArg->NumericByRef().u1 != 0 ? NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_TRUE
                                                                 : NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_FALSE;
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt8:
                *(*data)++ = instanceArg->NumericByRef().u1;
                break;

            case NF_Runtime_ISR_CompiledDataType::Int8:
                *(CLR_UINT8 *)(*data)++ = instanceArg->NumericByRef().s1;
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt16:
                *(CLR_UINT16 *)*data = instanceArg->NumericByRef().u2;
                *data += sizeof(CLR_UINT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int16:
                *(CLR_INT16 *)*data = instanceArg->NumericByRef().s2;
                *data += sizeof(CLR_INT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt32:
                *(CLR_UINT32 *)*data = instanceArg->NumericByRef().u4;
                *data += sizeof(CLR_UINT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int32:
                *(CLR_INT32 *)*data = instanceArg->NumericByRef().s4;
                *data += sizeof(CLR_INT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt64:
                *(CLR_UINT64 *)*data = (CLR_UINT64)instanceArg->NumericByRef().u8;
                *data += sizeof(CLR_UINT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int64:
                *(CLR_INT64 *)*data = (CLR_INT64)instanceArg->NumericByRef().s8;
                *data += sizeof(CLR_INT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Single:
                *(float *)*data = instanceArg->NumericByRef().r4;
                *data += sizeof(float);
                break;

            case NF_Runtime_ISR_CompiledDataType::Double:
                *(double *)*data = (double)instanceArg->NumericByRef().r8;
                *data += sizeof(double);
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_UNSUPPORTED_INSTRUCTION);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT ReadWriteManagedData(
    HRESULT (*readWrite)(CLR_RT_HeapBlock *instanceArg, CLR_UINT8 **dataType, bool asArrayElement, CLR_UINT8 **data),
    CLR_RT_HeapBlock &instanceArg,
    CLR_RT_HeapBlock &dataTypeArg,
    CLR_UINT8 *data)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataType = (CLR_UINT8 *)dataTypeArg.DereferenceArray()->GetFirstElement();
    if ((*dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0)
    {
        // instanceArg is an CLR_DataType::DATATYPE_SZARRAY representing the array.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg, &dataType, false, &data));
    }
    else if ((*dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an CLR_DataType::DATATYPE_OBJECT referencing a CLR_DataType::DATATYPE_VALUETYPE
        // for structs and a CLR_DataType::DATATYPE_CLASS for a class instance. As fields that point to
        // a struct/class instance also are of type CLR_DataType::DATATYPE_OBJECT, the CLR_DataType::DATATYPE_OBJECT
        // instance should be passed as argument.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg, &dataType, false, &data));
    }
    else
    {
        // instanceArg is an CLR_DataType::DATATYPE_OBJECT that references a CLR_DataType::DATATYPE_VALUETYPE
        // The value is the first field of the latter.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg.Dereference()[1], &dataType, false, &data));
    }

    NANOCLR_NOCLEANUP();
}