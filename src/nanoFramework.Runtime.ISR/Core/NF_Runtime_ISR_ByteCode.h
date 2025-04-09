//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//======================================================================
//
// This header for the ISR interpreter is shared by the nf-interpreter
// and the nanoFramework.Runtime.ISR.Interpreter and
// nanoFramework.Runtime.ISR.Tooling projects. If this file is modified
// in one git repository, make sure to copy it to to the other
// repository as well.
// 
//======================================================================

#if NF_RUNTIME_ISR_TOOLING
// Included in nanoFramework.Runtime.ISR.Tooling
namespace nanoFramework.Runtime.ISR.Compilation{
#endif

    // The data used by service routine is stored in three places:
    //
    // - A heap (one per service routine) for local variables defined in the service routine
    //   and for intermediate results during the evaluation of expressions.
    // - Shared ISR memory (optionally) for the class data fields, pointers to data bus implementations
    //   and buffers that are accessed by at least one service routine that is called in
    //   direct response to an interrupt.
    // - Shared RTOS task memory (optionally) for the class data fields, pointers to data bus implementations
    //   and buffers that are accessed by at least one service routine that is called in
    //   from a RTOS task started in response to an interrupt.
    // - Memory (optionally) for the class data fields, pointers to data bus implementations
    //   and buffers that are accessed by a service routine that is called from managed code
    //   instead of from an interrupt.
    //
    // For arrays the index of the element to use may not be known at compile time. Instead an indirect
    // reference is used: a location in the heap is used to compute the address of the element.
    //
    // As an optimization of the compiled byte code and size of the native interpreter the four
    // types of memory selection share a unified addressing scheme. If an opcode as a data location as argument:
    //
    // - If the first byte has a MSB of 1 (see NF_RUNTIME_ISR_MEMORY_HEAP_FLAG), the address
    //   is an offset in the heap memory. The address consists of 2 bytes (14 bits excluding the two MSB).
    //
    //      - If the one-less-than-MSB is 0, the 14-bit index points to the location the heap that should be used
    //        to evaluate the opcode.
    //      - If the one-less-than-MSB is 1, the 14-bit index points to the location the heap where a 2- or 4-byte
    //        data location is stored. The memory to use in the evaluation of the opcode is the memory pointed to
    //        by that data location.
    // 
    // - If the first byte has a MSB of 0, the address is an offset in shared memory.
    //   The address consists of 4 bytes (28 bits excluding the three MSB bits).
    // 
    //      - If the first byte has a one-less-than-MSB of 1, the 28-bit address points to a location
    //        where a 4-byte offset in the same shared memory is stored.  The memory to use in the evaluation
    //        of the opcode is the memory pointed to by that offset.
    //      - If the first byte has a two-and-three-less-than-MSB of 01, the 28-bit address
    //        is an offset in the shared ISR memory.
    //      - If the first byte has a two-and-three-less-than-MSB of 10, the 28-bit address
    //        is an offset in the shared RTOS task memory.
    //      - If the first byte has a two-and-three-less-than-MSB of 11, the 28-bit address
    //        is an offset in the memory for the service routine activated from managed code.
    // 

#if NF_RUNTIME_ISR_TOOLING
    /// <summary>
    /// Constants that describe aspects of the native implementation of the ISR features.
    /// </summary>
    public static class NativeImplementationConstants
    {
        /// <summary>
        /// Type to use to specify an offset of data in the shared memory.
        /// </summary>
        public const CompiledDataType SharedDataOffsetType = CompiledDataType.UInt32;

        /// <summary>
        /// Type to use to specify an offset of data in the heap of a routine.
        /// </summary>
        public const CompiledDataType HeapOffsetType = CompiledDataType.UInt16;

        /// <summary>
        /// Maximum size of a shared data memory block.
        /// </summary>
        public const int MaximumSharedDataSize = 0x10000000;

        /// <summary>
        /// Maximum size of the heap of a service routine.
        /// </summary>
        public const int MaximumHeapSize = 0x4000;

        /// <summary>
        /// Flag to add to an offset in the heap to get its unified memory address.
        /// </summary>
        public const int HeapOffsetFlag = 0x8000;

        /// <summary>
        /// Flag to add to an offset in the heap to indicate it is an indirect reference:
        /// the data at the heap location contains the actual data location.
        /// </summary>
        public const int HeapOffsetIndirectReferenceFlag = 0x4000;

        /// <summary>
        /// Bits in an unified memory address that show where the data is located.
        /// </summary>
        public const int SharedDataOffsetBits = 0x30000000;

        /// <summary>
        /// Flag to add to an offset in the shared data accessible from an ISR to get its unified memory address.
        /// </summary>
        public const int SharedISRDataOffsetFlag = 0x10000000;

        /// <summary>
        /// Flag to add to an offset in the shared data accessible from a RTOS task to get its unified memory address.
        /// </summary>
        public const int SharedTaskDataOffsetFlag = 0x20000000;

        /// <summary>
        /// Flag to add to an offset in the data accessible from a service routine activated from managed code to get its unified memory address.
        /// </summary>
        public const int ManagedActivationDataOffsetFlag = 0x30000000;

        /// <summary>
        /// Flag to add to an offset in the shared data to indicate it is an indirect reference:
        /// the data at the shared reference location contains the actual data location.
        /// </summary>
        public const int SharedDataOffsetIndirectReferenceFlag = 0x40000000;

        /// <summary>
        /// Type to use to specify a near offset within the compiled byte code of a service routine.
        /// </summary>
        public const CompiledDataType ByteCodeOffsetType = CompiledDataType.Int32;

        /// <summary>
        /// The maximum amount of bytes to reserve for a parameter to a service routine.
        /// </summary>
        public const CompiledDataType ServiceParameterType = CompiledDataType.UInt32;
}
#else
    typedef CLR_UINT32 NF_Runtime_ISR_SharedDataOffsetType;
    typedef CLR_UINT16 NF_Runtime_ISR_HeapOffsetType;
    typedef CLR_INT32 NF_Runtime_ISR_ByteCodeOffsetType;
    typedef CLR_UINT32 NF_Runtime_ISR_ServiceParameterType;

    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_HEAP_FLAG = 0x80;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_HEAP_MASK = (CLR_UINT8)~NF_RUNTIME_ISR_MEMORY_HEAP_FLAG;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_INDIRECT_REFERENCE_FLAG = 0x40;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_BITS = 0x30;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_ISR_FLAG = 0x10;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_TASK_FLAG = 0x20;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_ACTIVATION_FLAG = 0x30;
    constexpr CLR_UINT8 NF_RUNTIME_ISR_MEMORY_MASK = ~(CLR_UINT8)0x30;
    constexpr NF_Runtime_ISR_HeapOffsetType NF_RUNTIME_ISR_MEMORY_HEAP_INDIRECT_REFERENCE_FLAG = (((NF_Runtime_ISR_HeapOffsetType)NF_RUNTIME_ISR_INDIRECT_REFERENCE_FLAG) << 8);
    constexpr NF_Runtime_ISR_HeapOffsetType NF_RUNTIME_ISR_MEMORY_HEAP_INDIRECT_REFERENCE_MASK = ~NF_RUNTIME_ISR_MEMORY_HEAP_INDIRECT_REFERENCE_FLAG;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_OFFSET_BITS = ((NF_Runtime_ISR_SharedDataOffsetType)(NF_RUNTIME_ISR_MEMORY_BITS)) << 24;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_OFFSET_ISR_FLAG = ((NF_Runtime_ISR_SharedDataOffsetType)(NF_RUNTIME_ISR_MEMORY_ISR_FLAG)) << 24;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_OFFSET_TASK_FLAG = ((NF_Runtime_ISR_SharedDataOffsetType)(NF_RUNTIME_ISR_MEMORY_TASK_FLAG)) << 24;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_OFFSET_ACTIVATION_FLAG = ((NF_Runtime_ISR_SharedDataOffsetType)(NF_RUNTIME_ISR_MEMORY_ACTIVATION_FLAG)) << 24;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_OFFSET_MASK = ~NF_RUNTIME_ISR_MEMORY_OFFSET_BITS;
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_SHAREDDATA_INDIRECT_REFERENCE_FLAG = (((NF_Runtime_ISR_SharedDataOffsetType)NF_RUNTIME_ISR_INDIRECT_REFERENCE_FLAG) << 24);
    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MEMORY_SHAREDDATA_INDIRECT_REFERENCE_MASK = ~NF_RUNTIME_ISR_MEMORY_SHAREDDATA_INDIRECT_REFERENCE_FLAG;

    constexpr NF_Runtime_ISR_SharedDataOffsetType NF_RUNTIME_ISR_MAXIMUMSHAREDDATASIZE = 0x10000000;
#endif

#if NF_RUNTIME_ISR_TOOLING
    /// <summary>
    /// The supported data types for variables, class data fields and data struct fields.
    /// </summary>
    public enum CompiledDataType
#else
    enum class NF_Runtime_ISR_CompiledDataType : CLR_UINT8
#endif
    {
        Boolean = 1,
        UInt8,
        Int8,
        UInt16,
        Int16,
        UInt32,
        Int32,
        UInt64,
        Int64,
        Single,
        Double
    };

#if NF_RUNTIME_ISR_TOOLING
    /// <summary>
    /// The types of buffers supported by the ISR interpreter.
    /// </summary>
    public enum CompiledDataBufferTypes
#else
    enum class NF_Runtime_ISR_CompiledDataBufferType : CLR_UINT8
#endif
    {
        Buffer = 1,
        RingBuffer
    };

#if NF_RUNTIME_ISR_TOOLING
    /// <summary>
    /// The instruction set of the ISR interpreter.
    /// </summary>
    public enum CompiledOpCode
#else
    enum class NF_Runtime_ISR_CompiledOpCode : CLR_UINT8
#endif
    {
        // --------------------------------------------------------------------------------
        // Program flow
        // --------------------------------------------------------------------------------
        /// <summary>
        /// Return: stop executing this service routine but continue with other service routines
        /// (if multiple are specified).
        /// </summary>
        Return = 1,
        /// <summary>
        /// Abort: stop executing this service routine and do not continue with other service routines
        /// (if multiple are specified).
        /// </summary>
        Abort = 5,
        /// <summary>
        /// Return or abort: stop executing this service routine. First argument is the offset in unified memory
        /// to a boolean that decides whether (true) or not (false) to continue with other service routines
        /// (if multiple are specified).
        /// </summary>
        ReturnOrAbort = 6,
        /// <summary>
        /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType.
        /// </summary>
        GoTo = 2,
        /// <summary>
        /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType
        /// (first argument) if the condition is met. The second argument is the offset in unified memory where the
        /// boolean value of the condition is stored.
        /// </summary>
        OnConditionGoTo = 3,
        /// <summary>
        /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType
        /// (first argument) if the condition is not met. The second argument is the offset in unified memory where the
        /// boolean value of the condition is stored.
        /// </summary>
        OnNotConditionGoTo = 4,

        /// <summary>
        /// Raise an event that is handled by managed code. Argument is the offset in unified memory where the
        /// UInt32 value is stored that should be passed as an argument.
        /// </summary>
        ManagedEventRaise = 9,

        // --------------------------------------------------------------------------------
        // Memory operations
        // --------------------------------------------------------------------------------
        /// <summary>
        /// Load literal data into a data location. First argument is the offset in unified memory where the
        /// data should be stored, second is a byte with the number of bytes to load, followed by the bytes to load.
        /// </summary>
        StoreLiteral = 10,
        /// <summary>
        /// Fill the data in the data location with zeros. First argument is the offset in unified memory where the
        /// data should be stored, second is a HeapOffsetType with the number of bytes to assign zero to.
        /// </summary>
        StoreZero = 11,
        /// <summary>
        /// Copy data from one data location to another. First argument is the offset in unified memory where the
        /// data should be stored, second is the offset in unified memory where the data should be read from,
        /// third is a HeapOffsetType with the number of bytes to copy.
        /// </summary>
        CopyData = 12,
        /// <summary>
        /// Copy data from one data location to another, and change the (integer) type in the process. First argument
        /// is the offset in unified memory where the data should be stored, second is the offset in unified memory where the data should be read from,
        /// third is a byte with the type of the data to be read in the high nibble (0xF0) and the type of the data
        /// to be stored in the low nibble (0x0F).
        /// </summary>
        CopyTypecastData = 13,
        /// <summary>
        /// Copy the data to the heap that is passed as parameter to the service routine. First argument is a byte that
        /// is the number of bytes to copy.
        /// </summary>
        CopyParameter = 16,
        /// <summary>
        /// Update a reference to heap memory by adding an offset to it. First argument is the offset in the heap
        /// (as unified memory) where the reference resides, second is the offset in unified memory where the
        /// number of elements should be read from, and third is the element size. The number and size of elements
        /// are specified as the same type as the referenced stored in the heap: as heap offset type.
        /// </summary>
        UpdateHeapOffset = 14,
        /// <summary>
        /// Update a reference to heap memory by adding an offset to it. First argument is the offset in the heap
        /// (as unified memory) where the reference resides, second is the offset in unified memory where the
        /// number of elements should be read from, and third is the element size. The number and size of elements
        /// are specified as the same type as the referenced stored in the heap: as shared data offset type.
        /// </summary>
        UpdateSharedDataOffset = 15,

        // --------------------------------------------------------------------------------
        // Integer (and boolean) operations
        // --------------------------------------------------------------------------------
        /// <summary>
        /// Add the 1-byte integer from one data location to another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to add should be read from.
        /// </summary>
        AddInteger8 = 20,
        /// <summary>
        /// Add the 2-byte integer from one data location to another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to add should be read from.
        /// </summary>
        AddInteger16 = AddInteger8 + 1,
        /// <summary>
        /// Add the 4-byte integer from one data location to another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to add should be read from.
        /// </summary>
        AddInteger32 = AddInteger8 + 2,
        /// <summary>
        /// Add the 8-byte integer from one data location to another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to add should be read from.
        /// </summary>
        AddInteger64 = AddInteger8 + 3,
        /// <summary>
        /// Subtract the 1-byte integer from one data location from another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to subtract should be read from.
        /// </summary>
        SubtractInteger8 = AddInteger64 + 1,
        /// <summary>
        /// Subtract the 2-byte integer from one data location from another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to subtract should be read from.
        /// </summary>
        SubtractInteger16 = SubtractInteger8 + 1,
        /// <summary>
        /// Subtract the 4-byte integer from one data location from another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to subtract should be read from.
        /// </summary>
        SubtractInteger32 = SubtractInteger8 + 2,
        /// <summary>
        /// Subtract the 8-byte integer from one data location from another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to subtract should be read from.
        /// </summary>
        SubtractInteger64 = SubtractInteger8 + 3,
        /// <summary>
        /// Multiply the 1-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to multiply with should be read from.
        /// </summary>
        MultiplyInteger8 = SubtractInteger64 + 1,
        /// <summary>
        /// Multiply the 2-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to multiply with should be read from.
        /// </summary>
        MultiplyInteger16 = MultiplyInteger8 + 1,
        /// <summary>
        /// Multiply the 4-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to multiply with should be read from.
        /// </summary>
        MultiplyInteger32 = MultiplyInteger8 + 2,
        /// <summary>
        /// Multiply the 8-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to multiply with should be read from.
        /// </summary>
        MultiplyInteger64 = MultiplyInteger8 + 3,
        /// <summary>
        /// Divide the 1-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        DivideInteger8 = MultiplyInteger64 + 1,
        /// <summary>
        /// Divide the 2-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        DivideInteger16 = DivideInteger8 + 1,
        /// <summary>
        /// Divide the 4-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        DivideInteger32 = DivideInteger8 + 2,
        /// <summary>
        /// Divide the 8-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        DivideInteger64 = DivideInteger8 + 3,
        /// <summary>
        /// Calculate the remainder of the division of a 1-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        RemainderInteger8 = DivideInteger64 + 1,
        /// <summary>
        /// Calculate the remainder of the division of a 2-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        RemainderInteger16 = RemainderInteger8 + 1,
        /// <summary>
        /// Calculate the remainder of the division of a 4-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by by should be read from.
        /// </summary>
        RemainderInteger32 = RemainderInteger8 + 2,
        /// <summary>
        /// Calculate the remainder of the division of a 8-byte integer from one data location by another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to divide by should be read from.
        /// </summary>
        RemainderInteger64 = RemainderInteger8 + 3,
        /// <summary>
        /// Logical-and the 1-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-and should be read from.
        /// </summary>
        AndInteger8 = RemainderInteger64 + 1,
        /// <summary>
        /// Logical-and the 2-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-and should be read from.
        /// </summary>
        AndInteger16 = AndInteger8 + 1,
        /// <summary>
        /// Logical-and the 4-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-and should be read from.
        /// </summary>
        AndInteger32 = AndInteger8 + 2,
        /// <summary>
        /// Logical-and the 8-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-and should be read from.
        /// </summary>
        AndInteger64 = AndInteger8 + 3,
        /// <summary>
        /// Logical-or the 1-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-or should be read from.
        /// </summary>
        OrInteger8 = AndInteger64 + 1,
        /// <summary>
        /// Logical-or the 2-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-or should be read from.
        /// </summary>
        OrInteger16 = OrInteger8 + 1,
        /// <summary>
        /// Logical-or the 4-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-or should be read from.
        /// </summary>
        OrInteger32 = OrInteger8 + 2,
        /// <summary>
        /// Logical-or the 8-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-or should be read from.
        /// </summary>
        OrInteger64 = OrInteger8 + 3,
        /// <summary>
        /// Logical-xor the 1-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-xor should be read from.
        /// </summary>
        XorInteger8 = OrInteger64 + 1,
        /// <summary>
        /// Logical-xor the 2-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-xor should be read from.
        /// </summary>
        XorInteger16 = XorInteger8 + 1,
        /// <summary>
        /// Logical-xor the 4-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-xor should be read from.
        /// </summary>
        XorInteger32 = XorInteger8 + 2,
        /// <summary>
        /// Logical-xor the 8-byte integer from one data location with another. First argument is the offset in unified memory
        /// that should be updated, second is the offset in unified memory where the value to logical-xor should be read from.
        /// </summary>
        XorInteger64 = XorInteger8 + 3,
        /// <summary>
        /// Shift-left 1-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ShiftLeftInteger8 = XorInteger64 + 1,
        /// <summary>
        /// Shift-left 2-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ShiftLeftInteger16 = ShiftLeftInteger8 + 1,
        /// <summary>
        /// Shift-left 4-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ShiftLeftInteger32 = ShiftLeftInteger8 + 2,
        /// <summary>
        /// Shift-left 8-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ShiftLeftInteger64 = ShiftLeftInteger8 + 3,
        /// <summary>
        /// Shift-right 1-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        LogicalShiftRightInteger8 = ShiftLeftInteger64 + 1,
        /// <summary>
        /// Shift-right 2-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        LogicalShiftRightInteger16 = LogicalShiftRightInteger8 + 1,
        /// <summary>
        /// Shift-right 4-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        LogicalShiftRightInteger32 = LogicalShiftRightInteger8 + 2,
        /// <summary>
        /// Shift-right 8-byte integer from one data location by an amount of positions in the other location. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        LogicalShiftRightInteger64 = LogicalShiftRightInteger8 + 3,
        /// <summary>
        /// Shift-right 1-byte signed integer from one data location by an amount of positions in the other location, keeping its sign for negative values. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ArithmeticShiftRightInteger8 = LogicalShiftRightInteger64 + 1,
        /// <summary>
        /// Shift-right 2-byte signed integer from one data location by an amount of positions in the other location, keeping its sign for negative values. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ArithmeticShiftRightInteger16 = ArithmeticShiftRightInteger8 + 1,
        /// <summary>
        /// Shift-right 4-byte signed integer from one data location by an amount of positions in the other location, keeping its sign for negative values. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ArithmeticShiftRightInteger32 = ArithmeticShiftRightInteger8 + 2,
        /// <summary>
        /// Shift-right 8-byte signed integer from one data location by an amount of positions in the other location, keeping its sign for negative values. First argument
        /// is the offset in unified memory that should be updated, second is the offset in unified memory where the
        /// 1-byte number of positions to shift should be read from.
        /// </summary>
        ArithmeticShiftRightInteger64 = ArithmeticShiftRightInteger8 + 3,
        /// <summary>
        /// Bitwise complement the 1-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        ComplementInteger8 = ArithmeticShiftRightInteger64 + 1,
        /// <summary>
        /// Bitwise complement the 2-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        ComplementInteger16 = ComplementInteger8 + 1,
        /// <summary>
        /// Bitwise complement the 4-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        ComplementInteger32 = ComplementInteger8 + 2,
        /// <summary>
        /// Bitwise complement the 8-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        ComplementInteger64 = ComplementInteger8 + 3,
        /// <summary>
        /// Change the sign of the 1-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusInteger8 = ComplementInteger64 + 1,
        /// <summary>
        /// Change the sign of the 2-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusInteger16 = UnaryMinusInteger8 + 1,
        /// <summary>
        /// Change the sign of the 4-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusInteger32 = UnaryMinusInteger8 + 2,
        /// <summary>
        /// Change the sign of the 8-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusInteger64 = UnaryMinusInteger8 + 3,
        /// <summary>
        /// Subtract 1 off the 1-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusMinusInteger8 = UnaryMinusInteger64 + 1,
        /// <summary>
        /// Subtract 1 off the 2-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusMinusInteger16 = UnaryMinusMinusInteger8 + 1,
        /// <summary>
        /// Subtract 1 off the 4-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusMinusInteger32 = UnaryMinusMinusInteger8 + 2,
        /// <summary>
        /// Subtract 1 off the 8-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryMinusMinusInteger64 = UnaryMinusMinusInteger8 + 3,
        /// <summary>
        /// Add 1 to the 1-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryPlusPlusInteger8 = UnaryMinusMinusInteger64 + 1,
        /// <summary>
        /// Add 1 to the 2-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryPlusPlusInteger16 = UnaryPlusPlusInteger8 + 1,
        /// <summary>
        /// Add 1 to the 4-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryPlusPlusInteger32 = UnaryPlusPlusInteger8 + 2,
        /// <summary>
        /// Add 1 to the 8-byte integer at the data location. Argument is the offset in unified memory that should be updated.
        /// </summary>
        UnaryPlusPlusInteger64 = UnaryPlusPlusInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte integer from one data location with another, and sets the result to true if and only if
        /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreEqualInteger8 = UnaryPlusPlusInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte integer from one data location with another, and sets the result to true if and only if
        /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreEqualInteger16 = AreEqualInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte integer from one data location with another, and sets the result to true if and only if
        /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreEqualInteger32 = AreEqualInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte integer from one data location with another, and sets the result to true if and only if
        /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreEqualInteger64 = AreEqualInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte integer from one data location with another, and sets the result to true if and only if
        /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreNotEqualInteger8 = AreEqualInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte integer from one data location with another, and sets the result to true if and only if
        /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreNotEqualInteger16 = AreNotEqualInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte integer from one data location with another, and sets the result to true if and only if
        /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreNotEqualInteger32 = AreNotEqualInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte integer from one data location with another, and sets the result to true if and only if
        /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        AreNotEqualInteger64 = AreNotEqualInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanUnsignedInteger8 = AreNotEqualInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanUnsignedInteger16 = IsLessThanUnsignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanUnsignedInteger32 = IsLessThanUnsignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanUnsignedInteger64 = IsLessThanUnsignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanSignedInteger8 = IsLessThanUnsignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanSignedInteger16 = IsLessThanSignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanSignedInteger32 = IsLessThanSignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessThanSignedInteger64 = IsLessThanSignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualUnsignedInteger8 = IsLessThanSignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualUnsignedInteger16 = IsLessOrEqualUnsignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualUnsignedInteger32 = IsLessOrEqualUnsignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualUnsignedInteger64 = IsLessOrEqualUnsignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualSignedInteger8 = IsLessOrEqualUnsignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualSignedInteger16 = IsLessOrEqualSignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualSignedInteger32 = IsLessOrEqualSignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is less than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsLessOrEqualSignedInteger64 = IsLessOrEqualSignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanUnsignedInteger8 = IsLessOrEqualSignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanUnsignedInteger16 = IsGreaterThanUnsignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanUnsignedInteger32 = IsGreaterThanUnsignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanUnsignedInteger64 = IsGreaterThanUnsignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanSignedInteger8 = IsGreaterThanUnsignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanSignedInteger16 = IsGreaterThanSignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanSignedInteger32 = IsGreaterThanSignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterThanSignedInteger64 = IsGreaterThanSignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualUnsignedInteger8 = IsGreaterThanSignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualUnsignedInteger16 = IsGreaterOrEqualUnsignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualUnsignedInteger32 = IsGreaterOrEqualUnsignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualUnsignedInteger64 = IsGreaterOrEqualUnsignedInteger8 + 3,
        /// <summary>
        /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualSignedInteger8 = IsGreaterOrEqualUnsignedInteger64 + 1,
        /// <summary>
        /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualSignedInteger16 = IsGreaterOrEqualSignedInteger8 + 1,
        /// <summary>
        /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualSignedInteger32 = IsGreaterOrEqualSignedInteger8 + 2,
        /// <summary>
        /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and only if
        /// the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean (1-byte integer) that should be updated,
        /// second and third are the offset in unified memory where the values to compare should be read from.
        /// </summary>
        IsGreaterOrEqualSignedInteger64 = IsGreaterOrEqualSignedInteger8 + 3,

        // --------------------------------------------------------------------------------
        // Data buffer operations
        // --------------------------------------------------------------------------------
        /// <summary>
        /// Return the capacity of a data buffer. First argument is the offset in unified memory to the (native) data
        /// allocated for the data buffer. Second argument is the offset in unified memory to the place the capacity should be stored in.
        /// </summary>
        DataBufferGetCapacity = 225,
        /// <summary>
        /// Return the number of elements present in the data buffer. First argument is the offset in unified memory to the (native) data
        /// allocated for the data buffer. Second argument is the offset in unified memory to the place the count should be stored in.
        /// </summary>
        DataBufferGetCount = DataBufferGetCapacity + 1,
        /// <summary>
        /// Add data to the data buffer. If the data buffer is full, no data is added. First argument is the offset in unified memory
        /// to the (native) data allocated for the data buffer. Second argument is the offset in unified memory to the data to be
        /// added to the buffer.
        /// </summary>
        DataBufferAdd = DataBufferGetCount + 1,
        /// <summary>
        /// Add data to the data buffer at a specific location. First argument is the offset in unified memory
        /// to the (native) data allocated for the data buffer.  Second is a SharedDataOffsetType with the position to insert the data in.
        /// Third argument is the offset in unified memory to the data to be added to the buffer. 
        /// </summary>
        DataBufferInsert = DataBufferAdd + 1,
        /// <summary>
        /// Get data from the data buffer at a specific location. First argument is the offset in unified memory
        /// to the (native) data allocated for the data buffer. Second is a SharedDataOffsetType with the position to get the data from.
        /// Third argument is the offset in unified memory to the place the data should be stored in. 
        /// </summary>
        DataBufferGet = DataBufferInsert + 1,
        /// <summary>
        /// Clear all data from the data buffer. Argument is the offset in unified memory to the (native) data allocated for the data buffer.
        /// </summary>
        DataBufferClear = DataBufferGet + 1,
        /// <summary>
        /// Return the capacity of a data ring buffer. First argument is the offset in unified memory to the (native) data
        /// allocated for the data buffer. Second argument is the offset in unified memory to the place the capacity should be stored in.
        /// </summary>
        DataRingBufferGetCapacity = DataBufferClear + 1,
        /// <summary>
        /// Indicates whether the ring buffer does not contain any elements to peek/pop. First argument is the offset in unified memory to the (native) data
        /// allocated for the data buffer. Second argument is the offset in unified memory to a boolean the return value should be stored in.
        /// </summary>
        DataRingBufferIsEmpty = DataRingBufferGetCapacity + 1,
        /// <summary>
        /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for the ring buffer.
        /// Second argument is the offset in unified memory to the data to be added to the buffer.
        /// </summary>
        DataRingBufferPushOverwrite = DataRingBufferIsEmpty + 1,
        /// <summary>
        /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for the ring buffer.
        /// Second argument is the offset in unified memory to the data to be added to the buffer. Third argument is the offset in unified memory
        /// that contains a boolean whether to allow to overwrite the buffer. 
        /// </summary>
        DataRingBufferPushNoReturn = DataRingBufferPushOverwrite + 1,
        /// <summary>
        /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for the ring buffer.
        /// Second argument is the offset in unified memory to the data to be added to the buffer. Third argument is the offset in unified memory
        /// that contains a boolean whether to allow to overwrite the buffer. Third argument is the offset in unified memory to a boolean
        /// to store the return value in.
        /// </summary>
        DataRingBufferPush = DataRingBufferPushNoReturn + 1,
        /// <summary>
        /// Return the next data from the ring buffer but do not remove it from the buffer. First argument is the offset in unified memory to the
        /// (native) data allocated for the ring buffer. Second argument is the offset in unified memory to the memory to receive the data
        /// from the buffer. Third argument is the offset in unified memory to a boolean to store the return value in.
        /// </summary>
        DataRingBufferPeek = DataRingBufferPush + 1,
        /// <summary>
        /// Return the next data from the ring buffer but do not remove it from the buffer. First argument is the offset in unified memory to the
        /// (native) data allocated for the ring buffer. Second argument is the offset in unified memory to the memory to receive the data
        /// from the buffer.
        /// </summary>
        DataRingBufferPeekNoReturn = DataRingBufferPeek + 1,
        /// <summary>
        /// Return the next data from the ring buffer and remove it from the buffer. First argument is the offset in unified memory to the
        /// (native) data allocated for the ring buffer. Second argument is the offset in unified memory to the memory to receive the data
        /// from the buffer. Third argument is the offset in unified memory to a boolean to store the return value in.
        /// </summary>
        DataRingBufferPop = DataRingBufferPeekNoReturn + 1,
        /// <summary>
        /// Return the next data from the ring buffer and remove it from the buffer. First argument is the offset in unified memory to the
        /// (native) data allocated for the ring buffer. Second argument is the offset in unified memory to the memory to receive the data
        /// from the buffer.
        /// </summary>
        DataRingBufferPopNoReturn = DataRingBufferPop + 1,
        /// <summary>
        /// Clear all data from the ring buffer. First argument is the offset in unified memory to the (native) data allocated for the ring buffer.
        /// </summary>
        DataRingBufferClear = DataRingBufferPopNoReturn + 1,

        // --------------------------------------------------------------------------------
        // Data bus operations
        // --------------------------------------------------------------------------------
        /// <summary>
        /// Call the Read method of a data bus. First argument is the offset in unified memory to the (native) data
        /// allocated for the data bus. Second argument is the offset in unified memory to the place the data should be stored
        /// that is read via the data bus. Third argument is a HeapOffsetType with the number of bytes to read.
        /// </summary>
        DataBusRead = 250,
        /// <summary>
        /// Call the Write method of a data bus. First argument is the offset in unified memory to the (native) data
        /// allocated for the data bus. Second argument is the offset in unified memory to the data that should be written
        /// to the data bus. Third argument is a HeapOffsetType with the number of bytes to write.
        /// </summary>
        DataBusWrite = 251,
        /// <summary>
        /// Call the WriteRead method of a data bus. First argument is the offset in unified memory to the (native) data
        /// allocated for the data bus. Second argument is the offset in unified memory to the data that should be written
        /// to the data bus. Third argument is a HeapOffsetType with the number of bytes to write. Fourth argument is the
        /// offset in unified memory to the place the data should be stored that is read via the data bus. Fifth argument
        /// is a HeapOffsetType with the number of bytes to read.
        /// </summary>
        DataBusWriteRead = 252,
        /// <summary>
        /// Same as <see cref="DataBusRead"/>, but the result of the Read method (other than the data that has been read)
        /// is also stored. The fourth argument is the offset in unified memory where the result must be stored. The data bus
        /// implementation knows the size of the result as it defines the result type in the specification.
        /// </summary>
        DataBusReadWithResult = 253,
        /// <summary>
        /// Same as <see cref="DataBusWrite"/>, but the result of the Write method is also stored. The fourth argument is
        /// the offset in unified memory where the result must be stored. The data bus
        /// implementation knows the size of the result as it defines the result type in the specification. 
        /// </summary>
        DataBusWriteWithResult = 254,
        /// <summary>
        /// Same as <see cref="DataBusWriteRead"/>, but the result of the WriteRead method (other than the data that has been read)
        /// is also stored. The sixth argument is the offset in unified memory where the result must be stored. The data bus
        /// implementation knows the size of the result as it defines the result type in the specification. 
        /// </summary>
        DataBusWriteReadWithResult = 255,
    };

#if NF_RUNTIME_ISR_TOOLING
}
#endif
