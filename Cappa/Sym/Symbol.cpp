/*
 * Symbol.cpp
 *
 *  Created on: Dec 2, 2020
 *      Author: chris
 */

#include <Sym/Symbol.h>
#include "Builtins.h"
void MaySymbol::clearParent() {
	assert("Parent symbol not already set!" && (bool) parSym);
	parSym = {};
}
void MaySymbol::setParent(MaySymbol& s) {
	assert("Parent already set!" && (bool) !parSym);
	parSym = &s;
}

std::optional<Symbol*> findShallow(Symbol* toCheck, StringRef symName) {
	if (toCheck->containsShallowSyms()) {
		for (Symbol* s: toCheck->getShallowContainedSymbols()) {
			if (s->getName() == symName) return s;
			auto opt = findShallow(s, symName);
			if (opt) return opt;
		}
	}
	return {};
}
std::optional<Symbol*> MaySymbol::findSymRec(StringRef symName) {
	auto opt_spec_res = findSym_Spec(symName);
	if (opt_spec_res) return opt_spec_res;
	if (!parSym) return std::optional<Symbol*>();
	MaySymbol* cSym = *parSym;
	while (true) {
		if (cSym->isSym()) {
			Symbol* sym = cSym->getSym();
			if (sym->getName() == symName) return sym;
			if (sym->containsSyms())
				for (Symbol* s: sym->getContainedSymbols())
					if (s->getName() == symName) return s;
			auto opt = findShallow(sym, symName);
			if (opt) return opt;
		}

		if (!cSym->hasParentSym())
			return std::optional<Symbol*>();
		cSym = cSym->getParentSym();
	}
	assert("unreachable");
}
std::optional<Symbol*> MaySymbol::findSym(StringRef symName) {
	for (Symbol* s: BuiltinSymbols)
		if (s->getName() == symName) return s;
	return findSymRec(symName);
}
void Symbol::addSymbol(Symbol& s) {
	if (!containedSyms) containedSyms.emplace(); //Construct the vector object
	containedSyms->push_back(&s);
	s.setParent(*this);
}

void Symbol::addShallowSymbol(Symbol& s) {
	if (!shallowContainedSyms) shallowContainedSyms.emplace();
	shallowContainedSyms->push_back(&s);
	s.setParent(*this);
}
