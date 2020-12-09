/*
 * Wrapper.h
 *	Various wrapper types for the LLVM equivelants.
 *	Most everything, save for builtins, should be using these and not underlying LLVM constructs
 *  Created on: Nov 23, 2020
 *      Author: chris
 */

#ifndef WRAPPER_H_
#define WRAPPER_H_
class WType;
class WFunction;
class WValue;
class WGeneric;
#include "llvm/IR/IRBuilder.h"
#include "Lib.h"
using namespace llvm;

class WValue {
	//Must *always* contain a pointer
	std::optional<Value*> storage;
protected:
	std::optional<WType*> type;
	WValue(){}
public:
	//Allocates a value an the stack with the type passed
	WValue(WType*);
	WValue(WType* elemType, ArrayRef<WValue*> arrayDims);
	WValue(Value*, WType* llt = nullptr);
	//Will *always* return a Pointer
	Value* getStorage();
	Value* loadLL();
	//The implicit cast algorithm:
	//If neither this or to is a ref, try to find a matching I() operator
	//If 'this' is a ref, by 'to' is not, load 'this' into an RValue, then cast that to 'to'
	//If this and to are both refs, for now fail, but TODO with class inheritance allowing casting
	//If this is not a ref, but 'to' is, fail. You cannot turn an RValue into an LValue so implicitly.
	std::optional<WValue*> castTo(WType* to);
	virtual WType* getType();

	bool isWType();
	bool isWFunction();
	WType* asWType();
	WFunction* asWFunction();
	virtual ~WValue() {}
};
//There is only 1 of any given WType, so checking equality can be done by a simple pointer comparison
//The only exception would be classes; 2 WTypes may have the same llvm::Type, but be defined as different classes and so have 2 different WType objects
class WType: public WValue {
public:
private:
	//Empty-type constructor (Should only be used for primitive types such as void)
	WType() {}
	//Direct LLVM Constructor
	WType(Type* llType): llType(llType) {}
	//Array/Ref Type Constructor (arrDepth == 0 means just a reference)
	WType(WType* refType, int arrDims = 0);
	//Function Type Constructor
	WType(WType* retType, ArrayRef<WType*> args);
	//Class Type Constructor
	WType(std::string name);
	//<arg types, map <ret types, function type>>
	static WType IntType;
	static WType BoolType;
	static WType DoubleType;
	static WType ByteType;
	static WType VoidType;
	static WType TypeType;
	static WType GenericType;
	//<Args, <Return Type, FType>
	static std::map<std::vector<WType*>, std::map<WType*, WType*>> FunctionTypes;
	static std::map<WType*, WType*> RefTypes;
	static std::map<WType*, WType*> ArrTypes;

	//Members
	std::optional<Type*> llType;
	std::optional<WType*> RefdType;
	std::vector<WFunction*> constructors;
	//Function Properties
		//Pair: <retType, argTypes>
		std::optional<std::pair<WType*, std::vector<WType*>>> functionProperties;
	//Class Properties
		//Map: <name, value>
		std::optional<std::string> className;
		std::optional<std::map<std::string, WValue*>> classContents;
	//Array Properties
		std::optional<WType*> containedArrType;
		std::optional<int> arrDepth;
public:
	Type* getLLType();
	std::optional<FunctionType*> getLLFnType();

	bool isIntTy();
	bool isBoolTy();
	bool isDoubleTy();
	bool isByteTy();
	bool isVoidTy();
	bool isTypeTy();
	bool isGenericTy();
	bool isFuncTy();
	bool isRefTy();
	bool isClassTy();
	bool isArrayTy();
	WType* getRefTo();
	//Get type from llvm type
	static WType* getTy(Type*);
	static WType* getIntTy();
	static WType* getBoolTy();
	static WType* getDoubleTy();
	static WType* getByteTy();
	static WType* getVoidTy();
	static WType* getGenericTy();
	static WType* getClassTy(std::string name);
	static WType* getFuncTy(WType* retType, ArrayRef<WType*> argTypes);
	static WType* getRefTy(WType* ref);
	static WType* getArrayTy(WType*);

	//All types
	void addConstructor(WFunction*);
	std::vector<WFunction*>& getConstructors();

	//FnType
	ArrayRef<WType*> getFnArgs();
	WType* getFnArgTy(int n);
	WType* getFnRetTy();
	size_t getFnArgCt();

	//RefType
	WType* getTypeRefd();

	//Class Type
	void setClassContents(std::map<std::string, WValue*>&);
	const std::map<std::string, WValue*>& getClassContents();
	std::optional<std::pair<int, WType*>> getVar(StringRef varName);

	//Array Type
	WType* getArrayElemType();

	void dump();

	//Returns if there is an I() operator defined which takes 'this' as an argument and returns a type of 'to'
	bool canImplicitCastTo(WType* to);
	/* From WValue */
	virtual WType* getType();
};
/*
 * A note on function calling convention in the LLVM implementation
 * The function must take a ptr to the object
 * It is the job of the caller to perform an pre-call copying
 */
class WFunction: public WValue {
	std::optional<Function*> F;
	std::optional<std::vector<WValue*>> methodClassRefs;
	std::optional<std::function<WType* (ArrayRef<WType*>)>> genericGenerator;

	//List of all generics created using this generic
	//opt < map < inputTypes, outputType > >
	std::optional<std::map<std::vector<WType*>, WType*>> genericImpls;
	std::string name;
	bool allPathsDoExit = false;
public:
	//For actual functions
	WFunction(Function* F, StringRef name, WType* ftype, bool isClassMethod = false);
	//Generics
	WFunction(StringRef name, std::function<WType* (ArrayRef<WType*>)> genericGenerator);
	Function* getLLFunction();
	FunctionType* getLLFType();
	std::vector<WType*> getArgs();
	WType* getArgTy(int n);
	WType* getRetTy();
	size_t getArgCt();

	bool doAllPathsExit();
	void setAllPathsDoExit();

	bool canCall(ArrayRef<WType*>);
	std::optional<WValue*> call(ArrayRef<WValue*>);

	bool isGeneric();
	bool isFunction();
};

//Generate the LLVM array type given a WType, the # of dimensions, and optionally the specified array dimensions
StructType* getLLArrayType(WType*, int , ArrayRef<int> = {});

//Picks the first 'perfectly-matching' function from funcs
//A function is 'perfectly-matching' if, and only if, all arguments either have identical types, or the arg passed is a reference to the function's corresponding parameter
std::optional<WFunction*> getPerfectFunctionMatch(ArrayRef<WFunction*> funcs, ArrayRef<WType*> args);


#endif /* WRAPPER_H_ */
