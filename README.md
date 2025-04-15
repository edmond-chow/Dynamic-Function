# Dynamic-Function
The shared library allocating an executable heap, you can read, write or execute
some object code at runtime with this project.

# Interfaces
The infrastructures, fn_malloc, fn_realloc and fn_free are used for directly get
accessing the executable heap. The specializes including struct argument_traits,
function_proto and function_traits deduce from a concrete function type with a
calling convention involved all the argument types in parameter list and its
return type. The struct make_function_type backs to the its concrete function
type from its template arguments. The fn_call likewise what we have std::invoke
transfers control from an arbitrary type pointer. The functionalities consist of
struct byte and function. The struct byte overrides operator new and delete
through the allocations to wrap the executable heap.

# Implementations
In x86 mode, functions as such with thiscall likewise what we have in fastcall,
while both the callee the responsibility to clean up the stack for arguments
passed through by the caller, but fastcall using both ecx and edx registers pass
through the first 2 dword arguments while thiscall only using ecx register pass
through that pointer. As we inject the 2nd parameter of type 'int' to shift the
legacy of the parameters all pushed onto the stack if needed, whenever functions
are invoked where the edx register left as unspecified, preventing the abuse of
injection on object codes or inline assembly. The variadic-calls propagating the
arguments, the cdecl requires caller clean up the stack for parameter passing.
The rest just clean up inside callee by the last ret instruction with offset.

In x64 mode, such functions with thiscall is same as cdecl, while the this
argument treated as the first implicit parameter in which that pointer is
correspondence with rcx register to pass through with the default x64 calling
convention. The rest other than vector call also comply with the default x64
calling convention.
