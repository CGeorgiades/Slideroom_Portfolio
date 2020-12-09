/*
 * Symbol.h
 *
 *  Created on: Dec 2, 2020
 *      Author: chris
 */

#ifndef SYM_SYMBOL_H_
#define SYM_SYMBOL_H_
class Symbol;
class MaySymbol;
#include "Lib.h"
class Visitable; //From Visitor.h
class WValue; //From Wrapper.h
//Object that may, or may not, be a symbol
class MaySymbol {
	std::optional<MaySymbol*> parSym = std::optional<MaySymbol*>();
	//Recursive check for symbol; does not check any builtin symbols
	std::optional<Symbol*> findSymRec(StringRef symName);
protected:
	//For any statement/whatever which has special symbol resolution rules (e.g. for loops)
	virtual std::optional<Symbol*> findSym_Spec(StringRef symName) {return {}; }
public:
	MaySymbol(): parSym(std::optional<MaySymbol*>()) {}
	bool hasParentSym() {return (bool) parSym;}
	MaySymbol* getParentSym() {assert("No parent symbol!" && (bool) parSym); return *parSym;}
	virtual bool isSym() {return false;}
	virtual Symbol* getSym() {fatal("Not a symbol!");}

	//Set this object's parent to s
	//Can only be called once for a symbol
	//Any overload *must* call the child operation
	virtual void setParent(MaySymbol& par);

	//Clear the parent from this object
	void clearParent();

	//Attempts to locate the symbol
	std::optional<Symbol*> findSym(StringRef symName);

	virtual ~MaySymbol() {}
};

//Symbol object
class Symbol: virtual public MaySymbol {
	std::string symName;
	std::optional<Visitable*> ASTObj;
	std::optional<WValue*> value;
	std::optional<std::vector<Symbol*>> containedSyms;
	//Shallow symbols are symbols which, while defined inside this symbol, are read alongside this symbol
	std::optional<std::vector<Symbol*>> shallowContainedSyms;
public:
	Symbol(StringRef symName): symName(symName) {}
	Symbol(StringRef symName, Visitable* ASTObj): symName(symName), ASTObj(ASTObj) {}
	StringRef getName() const {return symName;}

	bool hasAttatchedASTObj() {return (bool) ASTObj;}
	std::optional<Visitable*> getASTObj() {return ASTObj;}

	bool hasVal() {return (bool) value;}
	void setValue(WValue* wv) {assert("Value already set!" && !value); value = wv;}
	std::optional<WValue*> getVal() {return value;}

	bool containsSyms() {return (bool) containedSyms;}
	ArrayRef<Symbol*> getContainedSymbols() {assert("No contained symbols!" && (bool) containedSyms); return *containedSyms;}
	bool containsShallowSyms() {return (bool) shallowContainedSyms;}
	ArrayRef<Symbol*> getShallowContainedSymbols() {assert("No shallow symbols!" && (bool) shallowContainedSyms); return *shallowContainedSyms;}
	bool isSym() {return true;}
	Symbol* getSym() {return this;}
	//Add a symbol to this symbol
	void addSymbol(Symbol& s);
	void addShallowSymbol(Symbol& s);

};

#endif /* SYM_SYMBOL_H_ */
