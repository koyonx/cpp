#include "AMateria.hpp"
#include "Ice.hpp"
#include "Cure.hpp"
#include "Fire.hpp"
#include "ICharacter.hpp"
#include "Character.hpp"
#include "IMateriaSource.hpp"
#include "MateriaSource.hpp"

static void print_header(const char* title)
{
	std::cout << std::endl << "===== " << title << " =====" << std::endl;
}

int main()
{
	// =========================================================================
	// 1) Subject's official example (output must match exactly)
	// =========================================================================
	{
		IMateriaSource* src = new MateriaSource();
		src->learnMateria(new Ice());
		src->learnMateria(new Cure());
		ICharacter* me = new Character("me");
		AMateria* tmp;
		tmp = src->createMateria("ice");
		me->equip(tmp);
		tmp = src->createMateria("cure");
		me->equip(tmp);
		ICharacter* bob = new Character("bob");
		me->use(0, *bob);
		me->use(1, *bob);
		delete bob;
		delete me;
		delete src;
	}

	// =========================================================================
	// 2) Full inventory: 5th equip is ignored.
	//    Caller is responsible for releasing the extra (subject hint).
	// =========================================================================
	print_header("Full inventory: 5th equip is ignored, caller frees extra");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		Character me("hero");

		for (int i = 0; i < 4; i++)
			me.equip(src.createMateria("ice"));

		AMateria* overflow = src.createMateria("ice");
		me.equip(overflow);
		std::cout << "5th equip ignored, caller must delete the extra materia" << std::endl;
		delete overflow;
	}

	// =========================================================================
	// 3) Unknown materia type and NULL safety in equip.
	// =========================================================================
	print_header("createMateria with unknown type returns 0; equip(NULL) is safe");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		AMateria* unknown = src.createMateria("lightning");
		std::cout << "createMateria(\"lightning\") returned: " << (unknown ? "non-null" : "NULL")
			<< std::endl;

		Character c("guinea-pig");
		c.equip(unknown);
		c.equip(NULL);
		std::cout << "equip(NULL) and equip(unknown=NULL) did not crash" << std::endl;
	}

	// =========================================================================
	// 4) Out of range use() / unequip(). Must not crash.
	// =========================================================================
	print_header("Out of range and empty-slot use/unequip");
	{
		Character me("alice");
		Character bob("bob");
		me.use(-1, bob);
		me.use(0, bob);   // slot is empty
		me.use(4, bob);
		me.use(100, bob);
		me.unequip(-1);
		me.unequip(4);
		me.unequip(0);    // slot is empty
		std::cout << "All invalid calls returned safely (no crash)" << std::endl;
	}

	// =========================================================================
	// 5) Character deep copy: copy constructor.
	//    Mutating the source must not affect the copy.
	// =========================================================================
	print_header("Character deep copy via copy constructor");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		src.learnMateria(new Cure());

		Character original("original");
		AMateria* originalSlot0 = src.createMateria("ice");
		original.equip(originalSlot0);
		original.equip(src.createMateria("cure"));

		Character copy(original);

		Character dummy("dummy");
		std::cout << "original.use(0,dummy): ";
		original.use(0, dummy);
		std::cout << "copy.use(0,dummy):     ";
		copy.use(0, dummy);

		// Save address, unequip original's slot 0, then free it ourselves
		// to avoid the leak the subject explicitly warns about.
		original.unequip(0);
		delete originalSlot0;
		std::cout << "After original.unequip(0) + delete, copy.use(0,dummy): ";
		copy.use(0, dummy);
		std::cout << "  ^ copy's slot 0 still works => deep copy" << std::endl;
	}

	// =========================================================================
	// 6) Character deep copy: assignment operator.
	//    Existing inventory must be released before overwriting (subject).
	// =========================================================================
	print_header("Character deep copy via assignment operator");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		src.learnMateria(new Cure());

		Character a("A");
		Character b("B");
		a.equip(src.createMateria("ice"));
		a.equip(src.createMateria("cure"));
		b.equip(src.createMateria("ice"));

		std::cout << "Before b = a: b.getName() = " << b.getName() << std::endl;
		std::cout << "--- b = a ---" << std::endl;
		b = a;
		std::cout << "After  b = a: b.getName() = " << b.getName()
			<< " (deep copied from a)" << std::endl;
		Character target("target");
		std::cout << "b.use(1,target) (should be cure): ";
		b.use(1, target);
		std::cout << "  ^ b's old ice was deleted before the new inventory was set" << std::endl;
	}

	// =========================================================================
	// 7) Save address before unequip to avoid leak.
	// =========================================================================
	print_header("Save address before unequip => no leak");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		Character me("careful");

		AMateria* ice = src.createMateria("ice");
		me.equip(ice);
		me.unequip(0);
		// We kept `ice` -> we own it now. Use it then delete.
		Character target("dummy");
		ice->use(target);
		delete ice;
		std::cout << "unequipped materia was reused and deleted explicitly" << std::endl;
	}

	// =========================================================================
	// 8) MateriaSource holds up to 4 templates, duplicates allowed.
	//    learnMateria clones the input and deletes it, so 5th call leaks nothing.
	// =========================================================================
	print_header("MateriaSource: up to 4 templates, duplicates allowed, no leak on overflow");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		src.learnMateria(new Ice());   // duplicates allowed
		src.learnMateria(new Cure());
		src.learnMateria(new Cure());
		src.learnMateria(new Ice());   // 5th: dropped by source AND freed by source

		AMateria* probeIce = src.createMateria("ice");
		std::cout << "createMateria(\"ice\") => " << probeIce->getType() << std::endl;
		delete probeIce;
	}

	// =========================================================================
	// 9) Custom AMateria subclass (Fire): inventory must support any AMateria.
	// =========================================================================
	print_header("Custom subclass Fire: inventory is type-agnostic");
	{
		MateriaSource src;
		src.learnMateria(new Fire());
		src.learnMateria(new Ice());

		Character me("wizard");
		me.equip(src.createMateria("fire"));
		me.equip(src.createMateria("ice"));

		Character bob("bob");
		me.use(0, bob);
		me.use(1, bob);
	}

	// =========================================================================
	// 10) Self assignment safety.
	// =========================================================================
	print_header("Character self-assignment is a no-op");
	{
		MateriaSource src;
		src.learnMateria(new Ice());
		Character me("selfie");
		me.equip(src.createMateria("ice"));

		me = me;
		Character bob("bob");
		std::cout << "After me = me, me.use(0,bob): ";
		me.use(0, bob);
	}

	// =========================================================================
	// 11) AMateria::operator= no longer copies type (subject hint).
	// =========================================================================
	print_header("AMateria::operator= does not overwrite type");
	{
		Ice ice;
		Cure cure;
		std::cout << "Before: ice.getType()=" << ice.getType()
			<< "  cure.getType()=" << cure.getType() << std::endl;
		// The conceptually wrong operation: trying to assign Cure into Ice.
		// We exercise it via the AMateria base operator to demonstrate that
		// the concrete type is preserved.
		AMateria& baseIce = ice;
		const AMateria& baseCure = cure;
		baseIce = baseCure;
		std::cout << "After ice = cure (via AMateria&):" << std::endl;
		std::cout << "  ice.getType()  = " << ice.getType()
			<< "  (still \"ice\": identity preserved)" << std::endl;
		std::cout << "  cure.getType() = " << cure.getType() << std::endl;
	}

	std::cout << std::endl << "===== All tests completed =====" << std::endl;
	return 0;
}
