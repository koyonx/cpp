#include "ClapTrap.hpp"
#include "ScavTrap.hpp"

void printSeparator(const std::string& title) {
    std::cout << "\n";
    std::cout << "================================================" << std::endl;
    std::cout << " " << title << std::endl;
    std::cout << "================================================" << std::endl;
}

void printTestHeader(int num, const std::string& description) {
    std::cout << "\n[TEST " << num << "] " << description << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

void printExpected(const std::string& msg) {
    std::cout << "  EXPECTED: " << msg << std::endl;
}

void printAction(const std::string& msg) {
    std::cout << "  >> " << msg << std::endl;
}

void printNote(const std::string& msg) {
    std::cout << "  NOTE: " << msg << std::endl;
}

int main(void) {
    printSeparator("SCAVTRAP COMPREHENSIVE TEST SUITE");
    std::cout << "Testing: Inheritance, Construction Chain, Polymorphism" << std::endl;
    std::cout << "ClapTrap Stats: HP=10,  EP=10,  AD=0" << std::endl;
    std::cout << "ScavTrap Stats: HP=100, EP=50, AD=20" << std::endl;

    printTestHeader(1, "ClapTrap Basic Functionality Review");
    printExpected("ClapTrap constructor, basic operations");
    ClapTrap clap1("ClapBasic");
    printAction("Attack (AD=0, EP: 10->9)");
    clap1.attack("Target");
    printAction("Take 5 damage (HP: 10->5)");
    clap1.takeDamage(5);
    printAction("Repair 3 HP (HP: 5->8, EP: 9->8)");
    clap1.beRepaired(3);

    printTestHeader(2, "ScavTrap Construction Chain");
    printExpected("ClapTrap constructor FIRST, then ScavTrap constructor");
    printNote("Watch for TWO constructor messages");
    ScavTrap scav1("ScavAlpha");

    printTestHeader(3, "ScavTrap Attack - Different Message");
    printExpected("ScavTrap-specific attack message (not ClapTrap's)");
    printNote("Damage should be 20 (not 0), EP: 50->49");
    printAction("ScavTrap attacks 'Enemy1'");
    scav1.attack("Enemy1");
    printAction("ScavTrap attacks 'Enemy2'");
    scav1.attack("Enemy2");

    printTestHeader(4, "ScavTrap Special Ability - guardGate()");
    printExpected("Gate keeper mode message");
    printNote("This is ScavTrap-specific, not in ClapTrap");
    scav1.guardGate();

    printTestHeader(5, "ScavTrap HP Test (100 HP vs ClapTrap's 10)");
    printExpected("ScavTrap can take more damage than ClapTrap");
    ScavTrap scav2("ScavTank");
    printAction("Take 30 damage (HP: 100->70)");
    scav2.takeDamage(30);
    printAction("Take another 30 damage (HP: 70->40)");
    scav2.takeDamage(30);
    printAction("Still operational, can attack");
    scav2.attack("StillAlive");
    printAction("Repair 25 HP (HP: 40->65, EP: 49->48)");
    scav2.beRepaired(25);

    printTestHeader(6, "ScavTrap Copy Constructor (OCF)");
    printExpected("Both ClapTrap AND ScavTrap copy constructors called");
    printNote("Watch for TWO copy constructor messages");
    ScavTrap scav2Copy(scav2);
    printAction("Copy attacks");
    scav2Copy.attack("CopyTarget");
    printAction("Copy uses guardGate");
    scav2Copy.guardGate();

    printTestHeader(7, "ScavTrap Assignment Operator (OCF)");
    printExpected("Both ClapTrap AND ScavTrap assignment operators called");
    ScavTrap scav3("ScavTemp");
    scav3.attack("PreAssignment");
    printAction("Assigning scav2 to scav3");
    scav3 = scav2;
    printAction("Assigned ScavTrap attacks");
    scav3.attack("PostAssignment");

    printTestHeader(8, "Self-Assignment Test");
    printExpected("No crash, proper self-assignment handling");
    printAction("scav3 = scav3");
    ScavTrap& scav3Ref = scav3;
    scav3 = scav3Ref;
    scav3.attack("AfterSelfAssign");

    printTestHeader(9, "ScavTrap Energy Test (50 EP)");
    printExpected("Can perform 50 actions before depletion");
    ScavTrap scavEnergy("ScavEnergetic");
    printAction("Performing 25 attacks");
    for (int i = 1; i <= 25; ++i) {
        scavEnergy.attack("Target");
    }
    printNote("EP should be at 25 now");
    printAction("guardGate doesn't consume EP");
    scavEnergy.guardGate();
    printAction("Performing 20 more attacks (EP: 25->5)");
    for (int i = 1; i <= 20; ++i) {
        scavEnergy.attack("Target");
    }
    printAction("5 more attacks to deplete (EP: 5->0)");
    for (int i = 1; i <= 5; ++i) {
        std::cout << "  Attack #" << (45 + i) << ": ";
        scavEnergy.attack("Target");
    }
    printExpected("Next attack should FAIL (EP=0)");
    scavEnergy.attack("ShouldFail");

    printTestHeader(10, "ScavTrap HP Depletion Test");
    printExpected("HP goes to 0, actions fail except guardGate");
    ScavTrap scavHP("ScavFragile");
    printAction("Taking 100 damage (HP: 100->0)");
    scavHP.takeDamage(100);
    printExpected("Attack should FAIL (HP=0)");
    scavHP.attack("ShouldFail");
    printExpected("Repair should FAIL (HP=0)");
    scavHP.beRepaired(10);
    printNote("guardGate might still work (no HP requirement in subject)");
    scavHP.guardGate();

    printTestHeader(11, "ScavTrap Overkill Damage");
    printExpected("HP doesn't go negative");
    ScavTrap scavOver("ScavOverkill");
    printAction("Taking 9999 damage (HP: 100->0)");
    scavOver.takeDamage(9999);
    printAction("Attempting action");
    scavOver.attack("Fail");

    printTestHeader(12, "ClapTrap vs ScavTrap Comparison");
    printExpected("Different stats and attack messages");
    ClapTrap clapComp("ClapComp");
    ScavTrap scavComp("ScavComp");
    printAction("ClapTrap attacks (AD=0)");
    clapComp.attack("Target");
    printAction("ScavTrap attacks (AD=20)");
    scavComp.attack("Target");
    printNote("Notice the different attack messages!");

    printTestHeader(13, "Inheritance Verification - Repair & Damage");
    printExpected("Inherited methods work correctly");
    ScavTrap scavInherit("ScavInherit");
    printAction("takeDamage (inherited from ClapTrap)");
    scavInherit.takeDamage(50);
    printAction("beRepaired (inherited from ClapTrap)");
    scavInherit.beRepaired(30);
    printNote("These use ClapTrap's implementation");

    printTestHeader(14, "Multiple ScavTraps Interaction");
    printExpected("Multiple ScavTraps can coexist");
    ScavTrap squad1("Squad1");
    ScavTrap squad2("Squad2");
    ScavTrap squad3("Squad3");
    printAction("Squad1 attacks");
    squad1.attack("Enemy");
    printAction("Squad2 attacks");
    squad2.attack("Enemy");
    printAction("Squad3 enters guard mode");
    squad3.guardGate();
    printAction("All take damage");
    squad1.takeDamage(20);
    squad2.takeDamage(20);
    squad3.takeDamage(20);

    printTestHeader(15, "ScavTrap After Heavy Combat");
    printExpected("ScavTrap survives what would kill ClapTrap");
    ScavTrap warrior("Warrior");
    printAction("Combat sequence: 10 attacks, heavy damage, repairs");
    for (int i = 0; i < 10; ++i) {
        warrior.attack("Enemy");
    }
    warrior.takeDamage(60);
    warrior.beRepaired(20);
    warrior.takeDamage(30);
    warrior.beRepaired(40);
    warrior.guardGate();
    printNote("ClapTrap would be dead by now!");

    printTestHeader(16, "Edge Case - Zero Values");
    printExpected("Zero damage/repair handled correctly");
    ScavTrap edge("EdgeScav");
    printAction("Take 0 damage");
    edge.takeDamage(0);
    printAction("Repair 0 HP (still consumes EP!)");
    edge.beRepaired(0);

    printTestHeader(17, "Edge Case - Maximum Repair");
    printExpected("Large repair values work");
    ScavTrap repair("RepairTest");
    repair.takeDamage(10);
    printAction("Repair 9999 HP");
    repair.beRepaired(9999);

    printTestHeader(18, "Rapid guardGate Calls");
    printExpected("guardGate can be called multiple times");
    ScavTrap guard("GuardTest");
    printAction("Calling guardGate 5 times");
    guard.guardGate();
    guard.guardGate();
    guard.guardGate();
    guard.guardGate();
    guard.guardGate();
    printNote("EP should be unchanged (no EP cost)");
    guard.attack("StillHasEnergy");

    printTestHeader(19, "Copy Independence Test");
    printExpected("Original and copy are independent");
    ScavTrap original("Original");
    original.attack("Test");
    original.takeDamage(20);
    ScavTrap copy(original);
    printAction("Original attacks again");
    original.attack("Test2");
    printAction("Copy attacks (different EP state)");
    copy.attack("Test3");
    printAction("Copy takes damage (shouldn't affect original)");
    copy.takeDamage(30);

    printTestHeader(20, "Assignment State Transfer");
    printExpected("State correctly transferred via assignment");
    ScavTrap source("Source");
    source.attack("A");
    source.attack("B");
    source.takeDamage(40);
    source.beRepaired(10);
    ScavTrap dest("Dest");
    dest.attack("PreAssign");
    printAction("Assigning source to dest");
    dest = source;
    printAction("Dest should have source's state (EP at 47)");
    dest.attack("PostAssign");

    printTestHeader(21, "Destruction Chain Test");
    printExpected("ScavTrap destructor FIRST, then ClapTrap destructor");
    printNote("Watch for TWO destructor messages (reverse order)");
    {
        ScavTrap temp("TempScav");
        printAction("Temp ScavTrap created in scope");
    }
    printNote("Temp should be destroyed here ^");

    printTestHeader(22, "Mixed Array Simulation");
    printExpected("Multiple ClapTraps and ScavTraps together");
    ClapTrap claps[3] = {
        ClapTrap("Clap1"),
        ClapTrap("Clap2"),
        ClapTrap("Clap3")
    };
    ScavTrap scavs[3] = {
        ScavTrap("Scav1"),
        ScavTrap("Scav2"),
        ScavTrap("Scav3")
    };
    printAction("All ClapTraps attack");
    for (int i = 0; i < 3; ++i) {
        claps[i].attack("Target");
    }
    printAction("All ScavTraps attack");
    for (int i = 0; i < 3; ++i) {
        scavs[i].attack("Target");
    }

    printTestHeader(23, "Stress Test - Alternating Actions");
    printExpected("Complex action sequences work correctly");
    ScavTrap stress("StressTest");
    for (int i = 0; i < 10; ++i) {
        stress.attack("E1");
        stress.beRepaired(1);
        stress.takeDamage(5);
        if (i % 3 == 0) stress.guardGate();
    }
    printNote("Should still be functional");
    stress.attack("FinalAttack");

    printSeparator("DESTRUCTION PHASE");
    std::cout << "All objects will be destroyed in reverse order of creation." << std::endl;
    std::cout << "Watch for PROPER DESTRUCTION CHAINS:" << std::endl;
    std::cout << "  - ScavTrap destructor first" << std::endl;
    std::cout << "  - ClapTrap destructor second" << std::endl;
    std::cout << "This is the reverse of construction order!" << std::endl;

    return 0;
}
