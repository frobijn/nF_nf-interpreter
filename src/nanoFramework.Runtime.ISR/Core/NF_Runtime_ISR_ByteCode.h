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
// clang-format off
// Included in nanoFramework.Runtime.ISR.Tooling
namespace nanoFramework.Runtime.ISR.Compilation
{
// clang-format on
#endif

// The data used by service routine is stored in four places:
//
// - Shared ISR memory (optionally) for the class data fields, pointers to data bus implementations
//   and buffers that are accessed by at least one service routine that is called in
//   direct response to an interrupt.
// - Shared RTOS task memory (optionally) for the class data fields, pointers to data bus implementations
//   and buffers that are accessed by at least one service routine that is called in
//   from a RTOS task started in response to an interrupt.
// - Memory (optionally) for the class data fields, pointers to data bus implementations
//   and buffers that are accessed by a service routine that is called from managed code
//   instead of from an interrupt.
// - A heap (if necessary: one for ISR, one for RTOS task, one for managed activated task)
//   for local variables defined in the service routine and for intermediate results during
//   the evaluation of expressions.
//
// For arrays the index of the element to use may not be known at compile time. Instead an indirect
// reference is used: a location in the heap is used to compute the address of the element.
//
// As an optimization of the compiled byte code and size of the native interpreter the four
// types of memory selection share a unified addressing scheme.
// If an opcode as a data location as argument:
//
// - The unified memory address is a 4-byte integer (little endian).
//      - The three MSB bits determine the location of the memory.
//      - The other 29 bits are an offset to the memory location w.r.t. the start of the memory.
//
// - If the MSB bit (0x80 00 00 00) is set, the address is an indirect reference. The address
//     points to a memory location that holds a unified memory address (with MSB bit = 0) that
//     should be used as the actual address of the memory location.
//     - If the address is pointing to the heap, the next two MSB bits of the actual address
//       are used.
//     - If the address is pointing to shared data, the actual address is the offset into
//       the same shared data memory.
//
// - The next two MSB bits indicate which memory the address points to:
//     - 00 = heap
//     - 01 = shared ISR memory
//     - 02 = shared RTOS task memory
//     - 03 = shared managed activation task memory

// Variables and class data fields declared in a *.isr.cs file can be of a native type
// (only the ones in CompiledDataType are supported), struct or (fixed-size) array of
// native type/struct. The memory reserved for a variable/field is a contiguous array
// of bytes. Structs are packed without filler bytes and arrays lack the length property.
// If a class (data or buffer) field is exposed as property to the regular managed code,
// a conversion/(de)serialization is required to/from the way the data is stored as
// managed objects.
//
// In a nanoFramework project, the conversion is done in native code because that is much
// faster than using reflection and byte-level conversions in managed code. To facilitate
// the conversion, the data type of a field or buffer element is described via an array of
// bytes. The array is read from the start; the value of a byte can be:
//
// - CompiledDataType
//          The data type is one of the supported native types.
// - CompiledDataType_IsArray + t
//          The data type is an array of t (= CompiledDataType or CompiledDataType_IsDataStruct + n).
// - CompiledDataType_IsDataStruct + n
//          The type is a data struct with n fields (n < CompiledDataType_MaxFields = 63).
//          The next bytes are n specifications of the fields. Each field specification starts
//          with a byte that is the index of the field within the struct, followed by the data
//          type of the field.
// 
// (For testing purposes using the full .NET framework the conversion is done in managed code
// using reflection and byte-level conversions as there is no other way.)

#if NF_RUNTIME_ISR_TOOLING
// clang-format off
/// <summary>
/// Constants that describe aspects of the native implementation of the ISR features.
/// </summary>
public static class NativeImplementationConstants
{
  /// <summary>
  /// Type to use to specify an offset of data in the heap and shared memory.
  /// </summary>
  public const CompiledDataType MemoryOffsetType = CompiledDataType.UInt32;

  /// <summary>
  /// Maximum size of the heap and of a shared data memory block.
  /// </summary>
  public const int MaximumMemoryBlockSize = 0x20_00_00_00;

  /// <summary>
  /// Flag to add to a memory offset to indicate it is an indirect reference:
  /// the data at the memory location contains the actual data location.
  /// </summary>
  public const uint UMAIndirectReferenceFlag = 0x80_00_00_00;

  /// <summary>
  /// Bits in an unified memory address that show where the data is located.
  /// </summary>
  public const uint UMAMemoryTypeMask = 0x60_00_00_00;

  /// <summary>
  /// Flag to add to an offset in the heap to get its unified memory address.
  /// </summary>
  public const uint UMAInHeapFlag = 0x00_00_00_00;

  /// <summary>
  /// Flag to add to an offset in the shared data accessible from an ISR to get its unified memory address.
  /// </summary>
  public const uint UMAInSharedISRDataFlag = 0x20_00_00_00;

  /// <summary>
  /// Flag to add to an offset in the shared data accessible from a RTOS task to get its unified memory address.
  /// </summary>
  public const uint UMAInSharedTaskDataFlag = 0x40_00_00_00;

  /// <summary>
  /// Flag to add to an offset in the data accessible from a service routine activated from managed code to get its
  /// unified memory address.
  /// </summary>
  public const uint UMAInManagedActivationDataFlag = 0x60_00_00_00;

  /// <summary>
  /// Type to use to specify a near offset within the compiled byte code of a service routine.
  /// </summary>
  public const CompiledDataType ByteCodeOffsetType = CompiledDataType.Int32;

  /// <summary>
  /// The maximum amount of bytes to reserve for a parameter to a service routine.
  /// </summary>
  public const CompiledDataType ServiceParameterType = CompiledDataType.UInt32;

  /// <summary>
  /// Indicates whether data is an array rather than a single value
  /// in the description of data for (de)serialization from byte array to managed data v.v..
  /// </summary>
  public const int CompiledDataType_IsArray = 0x80;

  /// <summary>
  /// Indicates whether the data type is a class or struct rather than a native type
  /// in the description of data for (de)serialization from byte array to managed data v.v..
  /// </summary>
  public const int CompiledDataType_IsDataStruct = 0x40;

  /// <summary>
  /// The maximum number of fields a data structure can have.
  /// </summary>
  public const int CompiledDataType_MaxFields = 0x3F;

  /// <summary>
  /// The binary value of the boolean <see langword="true"/> value.
  /// </summary>
  public const byte CompiledDataType_Boolean_True = 0xFF;
  /// <summary>
  /// The binary value of the boolean <see langword="false"/> value.
  /// </summary>
  public const byte CompiledDataType_Boolean_False = 0;
}
// clang-format on
#else
typedef CLR_UINT32 NF_Runtime_ISR_MemoryOffsetType;
typedef CLR_INT32 NF_Runtime_ISR_ByteCodeOffsetType;
typedef CLR_UINT32 NF_Runtime_ISR_ServiceParameterType;

constexpr NF_Runtime_ISR_ServiceParameterType NF_Runtime_ISR_ServiceParameterTypeSize =
    sizeof(NF_Runtime_ISR_ServiceParameterType);

typedef CLR_UINT32 NF_Runtime_ISR_UMA_Type;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_INDIRECT_REFERENCE_FLAG = 0x80000000;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_MEMORYTYPE_MASK = 0x60000000;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_OFFSET_MASK = ~(NF_RUNTIME_ISR_UMA_INDIRECT_REFERENCE_FLAG | NF_RUNTIME_ISR_UMA_MEMORYTYPE_MASK);
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_MEMORYTYPE_HEAP = 0x00000000;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_MEMORYTYPE_ISR = 0x20000000;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_MEMORYTYPE_RTOSTASK = 0x40000000;
constexpr CLR_UINT32 NF_RUNTIME_ISR_UMA_MEMORYTYPE_MANAGEDACTIVATION = 0x60000000;

constexpr NF_Runtime_ISR_MemoryOffsetType NF_RUNTIME_ISR_MAXIMUMSHAREDDATASIZE = 0x20000000;

constexpr CLR_UINT8 NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY = 0x80;
constexpr CLR_UINT8 NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT = 0x40;

constexpr CLR_UINT8 NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_TRUE = 0xFF;
constexpr CLR_UINT8 NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_FALSE = 0x00;
#endif

#if NF_RUNTIME_ISR_TOOLING
// clang-format off
/// <summary>
/// The supported data types for variables, class data fields and data struct fields.
/// </summary>
public enum CompiledDataType // KEEP IN SYNC WITH nanoCLR_DataType enum in nanoFramework.Tools.MetadataProcessor!!
// clang-format on
#else
enum class NF_Runtime_ISR_CompiledDataType : CLR_UINT8
#endif
{
  Boolean = 1,
  UInt8 = 3,
  Int8 = 2,
  UInt16 = 6,
  Int16 = 5,
  UInt32 = 8,
  Int32 = 7,
  UInt64 = 11,
  Int64 = 10,
  Single = 9,
  Double = 12
};

#if NF_RUNTIME_ISR_TOOLING
// clang-format off
/// <summary>
/// The types of buffers supported by the ISR interpreter.
/// </summary>
public enum CompiledDataBufferTypes
// clang-format on
#else
enum class NF_Runtime_ISR_CompiledDataBufferType : CLR_UINT8
#endif
{
    Buffer = 1,
    RingBuffer
};

#if NF_RUNTIME_ISR_TOOLING
// clang-format off
/// <summary>
/// The instruction set of the ISR interpreter.
/// </summary>
public enum CompiledOpCode
// clang-format on
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
    Abort = 2,
    /// <summary>
    /// Return or abort: stop executing this service routine. First argument is the offset in unified memory
    /// to a boolean that decides whether (true) or not (false) to continue with other service routines
    /// (if multiple are specified).
    /// </summary>
    ReturnOrAbort = 3,
    /// <summary>
    /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType.
    /// </summary>
    GoTo = 4,
    /// <summary>
    /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType
    /// (first argument) if the condition is met. The second argument is the offset in unified memory where the
    /// boolean value of the condition is stored.
    /// </summary>
    OnConditionGoTo = 5,
    /// <summary>
    /// Continue with the opcode at relative offset that is specified by a NF_Runtime_ISR_ByteCodeOffsetType
    /// (first argument) if the condition is not met. The second argument is the offset in unified memory where the
    /// boolean value of the condition is stored.
    /// </summary>
    OnNotConditionGoTo = 6,

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
    /// data should be stored, second is a MemoryOffsetType with the number of bytes to assign zero to.
    /// </summary>
    StoreZero = 11,
    /// <summary>
    /// Copy data from one data location to another. First argument is the offset in unified memory where the
    /// data should be stored, second is the offset in unified memory where the data should be read from,
    /// third is a MemoryOffsetType with the number of bytes to copy.
    /// </summary>
    CopyData = 12,
    /// <summary>
    /// Copy data from one data location to another, and change the (integer) type in the process. First argument
    /// is the offset in unified memory where the data should be stored, second is the offset in unified memory where
    /// the data should be read from, third is a byte with the type of the data to be read in the high nibble (0xF0) and
    /// the type of the data to be stored in the low nibble (0x0F).
    /// </summary>
    CopyTypecastData = 13,
    /// <summary>
    /// Copy the data to the heap that is passed as parameter to the service routine. First argument is a byte that
    /// is the number of bytes to copy.
    /// </summary>
    CopyParameter = 14,
    /// <summary>
    /// Update a reference to heap or shared data memory by adding an offset to it. First argument is the offset in the memory
    /// (as unified memory address) where the reference resides, second is the offset in unified memory where the
    /// number of elements should be read from, and third is the element size.
    /// </summary>
    UpdateMemoryOffset = 15,

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
    /// that should be updated, second is the offset in unified memory where the value to multiply with should be read
    /// from.
    /// </summary>
    MultiplyInteger8 = SubtractInteger64 + 1,
    /// <summary>
    /// Multiply the 2-byte integer from one data location with another. First argument is the offset in unified memory
    /// that should be updated, second is the offset in unified memory where the value to multiply with should be read
    /// from.
    /// </summary>
    MultiplyInteger16 = MultiplyInteger8 + 1,
    /// <summary>
    /// Multiply the 4-byte integer from one data location with another. First argument is the offset in unified memory
    /// that should be updated, second is the offset in unified memory where the value to multiply with should be read
    /// from.
    /// </summary>
    MultiplyInteger32 = MultiplyInteger8 + 2,
    /// <summary>
    /// Multiply the 8-byte integer from one data location with another. First argument is the offset in unified memory
    /// that should be updated, second is the offset in unified memory where the value to multiply with should be read
    /// from.
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
    /// Calculate the remainder of the division of a 1-byte integer from one data location by another. First argument is
    /// the offset in unified memory that should be updated, second is the offset in unified memory where the value to
    /// divide by should be read from.
    /// </summary>
    RemainderInteger8 = DivideInteger64 + 1,
    /// <summary>
    /// Calculate the remainder of the division of a 2-byte integer from one data location by another. First argument is
    /// the offset in unified memory that should be updated, second is the offset in unified memory where the value to
    /// divide by should be read from.
    /// </summary>
    RemainderInteger16 = RemainderInteger8 + 1,
    /// <summary>
    /// Calculate the remainder of the division of a 4-byte integer from one data location by another. First argument is
    /// the offset in unified memory that should be updated, second is the offset in unified memory where the value to
    /// divide by by should be read from.
    /// </summary>
    RemainderInteger32 = RemainderInteger8 + 2,
    /// <summary>
    /// Calculate the remainder of the division of a 8-byte integer from one data location by another. First argument is
    /// the offset in unified memory that should be updated, second is the offset in unified memory where the value to
    /// divide by should be read from.
    /// </summary>
    RemainderInteger64 = RemainderInteger8 + 3,
    /// <summary>
    /// Logical-and the 1-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-and should be
    /// read from.
    /// </summary>
    AndInteger8 = RemainderInteger64 + 1,
    /// <summary>
    /// Logical-and the 2-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-and should be
    /// read from.
    /// </summary>
    AndInteger16 = AndInteger8 + 1,
    /// <summary>
    /// Logical-and the 4-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-and should be
    /// read from.
    /// </summary>
    AndInteger32 = AndInteger8 + 2,
    /// <summary>
    /// Logical-and the 8-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-and should be
    /// read from.
    /// </summary>
    AndInteger64 = AndInteger8 + 3,
    /// <summary>
    /// Logical-or the 1-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-or should be
    /// read from.
    /// </summary>
    OrInteger8 = AndInteger64 + 1,
    /// <summary>
    /// Logical-or the 2-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-or should be
    /// read from.
    /// </summary>
    OrInteger16 = OrInteger8 + 1,
    /// <summary>
    /// Logical-or the 4-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-or should be
    /// read from.
    /// </summary>
    OrInteger32 = OrInteger8 + 2,
    /// <summary>
    /// Logical-or the 8-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-or should be
    /// read from.
    /// </summary>
    OrInteger64 = OrInteger8 + 3,
    /// <summary>
    /// Logical-xor the 1-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-xor should be
    /// read from.
    /// </summary>
    XorInteger8 = OrInteger64 + 1,
    /// <summary>
    /// Logical-xor the 2-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-xor should be
    /// read from.
    /// </summary>
    XorInteger16 = XorInteger8 + 1,
    /// <summary>
    /// Logical-xor the 4-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-xor should be
    /// read from.
    /// </summary>
    XorInteger32 = XorInteger8 + 2,
    /// <summary>
    /// Logical-xor the 8-byte integer from one data location with another. First argument is the offset in unified
    /// memory that should be updated, second is the offset in unified memory where the value to logical-xor should be
    /// read from.
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
    /// Shift-right 1-byte integer from one data location by an amount of positions in the other location. First
    /// argument is the offset in unified memory that should be updated, second is the offset in unified memory where
    /// the 1-byte number of positions to shift should be read from.
    /// </summary>
    LogicalShiftRightInteger8 = ShiftLeftInteger64 + 1,
    /// <summary>
    /// Shift-right 2-byte integer from one data location by an amount of positions in the other location. First
    /// argument is the offset in unified memory that should be updated, second is the offset in unified memory where
    /// the 1-byte number of positions to shift should be read from.
    /// </summary>
    LogicalShiftRightInteger16 = LogicalShiftRightInteger8 + 1,
    /// <summary>
    /// Shift-right 4-byte integer from one data location by an amount of positions in the other location. First
    /// argument is the offset in unified memory that should be updated, second is the offset in unified memory where
    /// the 1-byte number of positions to shift should be read from.
    /// </summary>
    LogicalShiftRightInteger32 = LogicalShiftRightInteger8 + 2,
    /// <summary>
    /// Shift-right 8-byte integer from one data location by an amount of positions in the other location. First
    /// argument is the offset in unified memory that should be updated, second is the offset in unified memory where
    /// the 1-byte number of positions to shift should be read from.
    /// </summary>
    LogicalShiftRightInteger64 = LogicalShiftRightInteger8 + 3,
    /// <summary>
    /// Shift-right 1-byte signed integer from one data location by an amount of positions in the other location,
    /// keeping its sign for negative values. First argument is the offset in unified memory that should be updated,
    /// second is the offset in unified memory where the 1-byte number of positions to shift should be read from.
    /// </summary>
    ArithmeticShiftRightInteger8 = LogicalShiftRightInteger64 + 1,
    /// <summary>
    /// Shift-right 2-byte signed integer from one data location by an amount of positions in the other location,
    /// keeping its sign for negative values. First argument is the offset in unified memory that should be updated,
    /// second is the offset in unified memory where the 1-byte number of positions to shift should be read from.
    /// </summary>
    ArithmeticShiftRightInteger16 = ArithmeticShiftRightInteger8 + 1,
    /// <summary>
    /// Shift-right 4-byte signed integer from one data location by an amount of positions in the other location,
    /// keeping its sign for negative values. First argument is the offset in unified memory that should be updated,
    /// second is the offset in unified memory where the 1-byte number of positions to shift should be read from.
    /// </summary>
    ArithmeticShiftRightInteger32 = ArithmeticShiftRightInteger8 + 2,
    /// <summary>
    /// Shift-right 8-byte signed integer from one data location by an amount of positions in the other location,
    /// keeping its sign for negative values. First argument is the offset in unified memory that should be updated,
    /// second is the offset in unified memory where the 1-byte number of positions to shift should be read from.
    /// </summary>
    ArithmeticShiftRightInteger64 = ArithmeticShiftRightInteger8 + 3,
    /// <summary>
    /// Bitwise complement the 1-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    ComplementInteger8 = ArithmeticShiftRightInteger64 + 1,
    /// <summary>
    /// Bitwise complement the 2-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    ComplementInteger16 = ComplementInteger8 + 1,
    /// <summary>
    /// Bitwise complement the 4-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    ComplementInteger32 = ComplementInteger8 + 2,
    /// <summary>
    /// Bitwise complement the 8-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    ComplementInteger64 = ComplementInteger8 + 3,
    /// <summary>
    /// Change the sign of the 1-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    UnaryMinusInteger8 = ComplementInteger64 + 1,
    /// <summary>
    /// Change the sign of the 2-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    UnaryMinusInteger16 = UnaryMinusInteger8 + 1,
    /// <summary>
    /// Change the sign of the 4-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    UnaryMinusInteger32 = UnaryMinusInteger8 + 2,
    /// <summary>
    /// Change the sign of the 8-byte integer at the data location. Argument is the offset in unified memory that should
    /// be updated.
    /// </summary>
    UnaryMinusInteger64 = UnaryMinusInteger8 + 3,
    /// <summary>
    /// Subtract 1 off the 1-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryMinusMinusInteger8 = UnaryMinusInteger64 + 1,
    /// <summary>
    /// Subtract 1 off the 2-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryMinusMinusInteger16 = UnaryMinusMinusInteger8 + 1,
    /// <summary>
    /// Subtract 1 off the 4-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryMinusMinusInteger32 = UnaryMinusMinusInteger8 + 2,
    /// <summary>
    /// Subtract 1 off the 8-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryMinusMinusInteger64 = UnaryMinusMinusInteger8 + 3,
    /// <summary>
    /// Add 1 to the 1-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryPlusPlusInteger8 = UnaryMinusMinusInteger64 + 1,
    /// <summary>
    /// Add 1 to the 2-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryPlusPlusInteger16 = UnaryPlusPlusInteger8 + 1,
    /// <summary>
    /// Add 1 to the 4-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryPlusPlusInteger32 = UnaryPlusPlusInteger8 + 2,
    /// <summary>
    /// Add 1 to the 8-byte integer at the data location. Argument is the offset in unified memory that should be
    /// updated.
    /// </summary>
    UnaryPlusPlusInteger64 = UnaryPlusPlusInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte integer from one data location with another, and sets the result to true if and only if
    /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreEqualInteger8 = UnaryPlusPlusInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte integer from one data location with another, and sets the result to true if and only if
    /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreEqualInteger16 = AreEqualInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte integer from one data location with another, and sets the result to true if and only if
    /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreEqualInteger32 = AreEqualInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte integer from one data location with another, and sets the result to true if and only if
    /// they are equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreEqualInteger64 = AreEqualInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte integer from one data location with another, and sets the result to true if and only if
    /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreNotEqualInteger8 = AreEqualInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte integer from one data location with another, and sets the result to true if and only if
    /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreNotEqualInteger16 = AreNotEqualInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte integer from one data location with another, and sets the result to true if and only if
    /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreNotEqualInteger32 = AreNotEqualInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte integer from one data location with another, and sets the result to true if and only if
    /// they are not equal. First argument is the offset in unified memory to a boolean (1-byte integer) that should be
    /// updated, second and third are the offset in unified memory where the values to compare should be read from.
    /// </summary>
    AreNotEqualInteger64 = AreNotEqualInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanUnsignedInteger8 = AreNotEqualInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanUnsignedInteger16 = IsLessThanUnsignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanUnsignedInteger32 = IsLessThanUnsignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanUnsignedInteger64 = IsLessThanUnsignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanSignedInteger8 = IsLessThanUnsignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanSignedInteger16 = IsLessThanSignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanSignedInteger32 = IsLessThanSignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsLessThanSignedInteger64 = IsLessThanSignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualUnsignedInteger8 = IsLessThanSignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualUnsignedInteger16 = IsLessOrEqualUnsignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualUnsignedInteger32 = IsLessOrEqualUnsignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualUnsignedInteger64 = IsLessOrEqualUnsignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than or equal to the second. First argument is the offset in unified memory to a boolean
    /// (1-byte integer) that should be updated, second and third are the offset in unified memory where the values to
    /// compare should be read from.
    /// </summary>
    IsLessOrEqualSignedInteger8 = IsLessOrEqualUnsignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is less than or equal to the second. First argument is the offset in unified memory to a boolean
    /// (1-byte integer) that should be updated, second and third are the offset in unified memory where the values to
    /// compare should be read from.
    /// </summary>
    IsLessOrEqualSignedInteger16 = IsLessOrEqualSignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualSignedInteger32 = IsLessOrEqualSignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is less than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsLessOrEqualSignedInteger64 = IsLessOrEqualSignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanUnsignedInteger8 = IsLessOrEqualSignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanUnsignedInteger16 = IsGreaterThanUnsignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanUnsignedInteger32 = IsGreaterThanUnsignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanUnsignedInteger64 = IsGreaterThanUnsignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanSignedInteger8 = IsGreaterThanUnsignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanSignedInteger16 = IsGreaterThanSignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanSignedInteger32 = IsGreaterThanSignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than the second. First argument is the offset in unified memory to a boolean (1-byte
    /// integer) that should be updated, second and third are the offset in unified memory where the values to compare
    /// should be read from.
    /// </summary>
    IsGreaterThanSignedInteger64 = IsGreaterThanSignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualUnsignedInteger8 = IsGreaterThanSignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualUnsignedInteger16 = IsGreaterOrEqualUnsignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualUnsignedInteger32 = IsGreaterOrEqualUnsignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualUnsignedInteger64 = IsGreaterOrEqualUnsignedInteger8 + 3,
    /// <summary>
    /// Compares the 1-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean
    /// (1-byte integer) that should be updated, second and third are the offset in unified memory where the values to
    /// compare should be read from.
    /// </summary>
    IsGreaterOrEqualSignedInteger8 = IsGreaterOrEqualUnsignedInteger64 + 1,
    /// <summary>
    /// Compares the 2-byte signed integer from one data location with another, and sets the result to true if and only
    /// if the first is greater than or equal to the second. First argument is the offset in unified memory to a boolean
    /// (1-byte integer) that should be updated, second and third are the offset in unified memory where the values to
    /// compare should be read from.
    /// </summary>
    IsGreaterOrEqualSignedInteger16 = IsGreaterOrEqualSignedInteger8 + 1,
    /// <summary>
    /// Compares the 4-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualSignedInteger32 = IsGreaterOrEqualSignedInteger8 + 2,
    /// <summary>
    /// Compares the 8-byte unsigned integer from one data location with another, and sets the result to true if and
    /// only if the first is greater than or equal to the second. First argument is the offset in unified memory to a
    /// boolean (1-byte integer) that should be updated, second and third are the offset in unified memory where the
    /// values to compare should be read from.
    /// </summary>
    IsGreaterOrEqualSignedInteger64 = IsGreaterOrEqualSignedInteger8 + 3,

    // --------------------------------------------------------------------------------
    // Data buffer operations
    // --------------------------------------------------------------------------------
    /// <summary>
    /// Return the capacity of a data buffer. First argument is the offset in unified memory to the (native) data
    /// allocated for the data buffer. Second argument is the offset in unified memory to the place the capacity should
    /// be stored in.
    /// </summary>
    DataBufferGetCapacity = 225,
    /// <summary>
    /// Return the number of elements present in the data buffer. First argument is the offset in unified memory to the
    /// (native) data allocated for the data buffer. Second argument is the offset in unified memory to the place the
    /// count should be stored in.
    /// </summary>
    DataBufferGetCount = DataBufferGetCapacity + 1,
    /// <summary>
    /// Add data to the data buffer. If the data buffer is full, no data is added. First argument is the offset in
    /// unified memory to the (native) data allocated for the data buffer. Second argument is the offset in unified
    /// memory to the data to be added to the buffer.
    /// </summary>
    DataBufferAdd = DataBufferGetCount + 1,
    /// <summary>
    /// Add data to the data buffer at a specific location. First argument is the offset in unified memory
    /// to the (native) data allocated for the data buffer.  Second is a MemoryOffsetType with the position to
    /// insert the data in. Third argument is the offset in unified memory to the data to be added to the buffer.
    /// </summary>
    DataBufferInsert = DataBufferAdd + 1,
    /// <summary>
    /// Get data from the data buffer at a specific location. First argument is the offset in unified memory
    /// to the (native) data allocated for the data buffer. Second is a MemoryOffsetType with the position to get
    /// the data from. Third argument is the offset in unified memory to the place the data should be stored in.
    /// </summary>
    DataBufferGet = DataBufferInsert + 1,
    /// <summary>
    /// Clear all data from the data buffer. Argument is the offset in unified memory to the (native) data allocated for
    /// the data buffer.
    /// </summary>
    DataBufferClear = DataBufferGet + 1,
    /// <summary>
    /// Return the capacity of a data ring buffer. First argument is the offset in unified memory to the (native) data
    /// allocated for the data buffer. Second argument is the offset in unified memory to the place the capacity should
    /// be stored in.
    /// </summary>
    DataRingBufferGetCapacity = DataBufferClear + 1,
    /// <summary>
    /// Indicates whether the ring buffer does not contain any elements to peek/pop. First argument is the offset in
    /// unified memory to the (native) data allocated for the data buffer. Second argument is the offset in unified
    /// memory to a boolean the return value should be stored in.
    /// </summary>
    DataRingBufferIsEmpty = DataRingBufferGetCapacity + 1,
    /// <summary>
    /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for
    /// the ring buffer. Second argument is the offset in unified memory to the data to be added to the buffer.
    /// </summary>
    DataRingBufferPushOverwrite = DataRingBufferIsEmpty + 1,
    /// <summary>
    /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for
    /// the ring buffer. Second argument is the offset in unified memory to the data to be added to the buffer. Third
    /// argument is the offset in unified memory that contains a boolean whether to allow to overwrite the buffer.
    /// </summary>
    DataRingBufferPushNoReturn = DataRingBufferPushOverwrite + 1,
    /// <summary>
    /// Add data to the ring buffer. First argument is the offset in unified memory to the (native) data allocated for
    /// the ring buffer. Second argument is the offset in unified memory to the data to be added to the buffer. Third
    /// argument is the offset in unified memory that contains a boolean whether to allow to overwrite the buffer. Third
    /// argument is the offset in unified memory to a boolean to store the return value in.
    /// </summary>
    DataRingBufferPush = DataRingBufferPushNoReturn + 1,
    /// <summary>
    /// Return the next data from the ring buffer but do not remove it from the buffer. First argument is the offset in
    /// unified memory to the (native) data allocated for the ring buffer. Second argument is the offset in unified
    /// memory to the memory to receive the data from the buffer. Third argument is the offset in unified memory to a
    /// boolean to store the return value in.
    /// </summary>
    DataRingBufferPeek = DataRingBufferPush + 1,
    /// <summary>
    /// Return the next data from the ring buffer but do not remove it from the buffer. First argument is the offset in
    /// unified memory to the (native) data allocated for the ring buffer. Second argument is the offset in unified
    /// memory to the memory to receive the data from the buffer.
    /// </summary>
    DataRingBufferPeekNoReturn = DataRingBufferPeek + 1,
    /// <summary>
    /// Return the next data from the ring buffer and remove it from the buffer. First argument is the offset in unified
    /// memory to the (native) data allocated for the ring buffer. Second argument is the offset in unified memory to
    /// the memory to receive the data from the buffer. Third argument is the offset in unified memory to a boolean to
    /// store the return value in.
    /// </summary>
    DataRingBufferPop = DataRingBufferPeekNoReturn + 1,
    /// <summary>
    /// Return the next data from the ring buffer and remove it from the buffer. First argument is the offset in unified
    /// memory to the (native) data allocated for the ring buffer. Second argument is the offset in unified memory to
    /// the memory to receive the data from the buffer.
    /// </summary>
    DataRingBufferPopNoReturn = DataRingBufferPop + 1,
    /// <summary>
    /// Clear all data from the ring buffer. First argument is the offset in unified memory to the (native) data
    /// allocated for the ring buffer.
    /// </summary>
    DataRingBufferClear = DataRingBufferPopNoReturn + 1,

    // --------------------------------------------------------------------------------
    // Data bus operations
    // --------------------------------------------------------------------------------
    /// <summary>
    /// Call the Read method of a data bus. First argument is the offset in unified memory to the (native) data
    /// allocated for the data bus. Second argument is the offset in unified memory to the place the data should be
    /// stored that is read via the data bus. Third argument is a MemoryOffsetType with the number of bytes to read.
    /// </summary>
    DataBusRead = 250,
    /// <summary>
    /// Call the Write method of a data bus. First argument is the offset in unified memory to the (native) data
    /// allocated for the data bus. Second argument is the offset in unified memory to the data that should be written
    /// to the data bus. Third argument is a MemoryOffsetType with the number of bytes to write.
    /// </summary>
    DataBusWrite = 251,
    /// <summary>
    /// Call the WriteRead method of a data bus. First argument is the offset in unified memory to the (native) data
    /// allocated for the data bus. Second argument is the offset in unified memory to the data that should be written
    /// to the data bus. Third argument is a MemoryOffsetType with the number of bytes to write. Fourth argument is the
    /// offset in unified memory to the place the data should be stored that is read via the data bus. Fifth argument
    /// is a MemoryOffsetType with the number of bytes to read.
    /// </summary>
    DataBusWriteRead = 252,
    /// <summary>
    /// Same as <see cref="DataBusRead"/>, but the result of the Read method (other than the data that has been read)
    /// is also stored. The fourth argument is the offset in unified memory where the result must be stored. The data
    /// bus implementation knows the size of the result as it defines the result type in the specification.
    /// </summary>
    DataBusReadWithResult = 253,
    /// <summary>
    /// Same as <see cref="DataBusWrite"/>, but the result of the Write method is also stored. The fourth argument is
    /// the offset in unified memory where the result must be stored. The data bus
    /// implementation knows the size of the result as it defines the result type in the specification.
    /// </summary>
    DataBusWriteWithResult = 254,
    /// <summary>
    /// Same as <see cref="DataBusWriteRead"/>, but the result of the WriteRead method (other than the data that has
    /// been read) is also stored. The sixth argument is the offset in unified memory where the result must be stored.
    /// The data bus implementation knows the size of the result as it defines the result type in the specification.
    /// </summary>
    DataBusWriteReadWithResult = 255,
};

#if NF_RUNTIME_ISR_TOOLING
}
#endif
