#include "ClapTrap.hpp"

void printSeparator(const std::string& title) {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << " " << title << std::endl;
    std::cout << "========================================" << std::endl;
}

void printTestHeader(int num, const std::string& description) {
    std::cout << "\n[TEST " << num << "] " << description << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

void printExpected(const std::string& msg) {
    std::cout << "  EXPECTED: " << msg << std::endl;
}

void printAction(const std::string& msg) {
    std::cout << "  >> " << msg << std::endl;
}

int main(void) {
    printSeparator("CLAPTRAP COMPREHENSIVE TEST SUITE");
    std::cout << "Testing: Constructor, Destructor, Attack, Repair, Damage" << std::endl;
    std::cout << "Initial Stats: HP=10, EP=10, AD=0" << std::endl;

    printTestHeader(1, "Default & Parameterized Constructor");
    printExpected("Constructor messages for 'Alpha' and 'Beta'");
    ClapTrap alpha("Alpha");
    ClapTrap beta("Beta");

    printTestHeader(2, "Copy Constructor (Orthodox Canonical Form)");
    printExpected("Copy constructor message, copying 'Alpha'");
    ClapTrap alphaCopy(alpha);

    printTestHeader(3, "Assignment Operator (Orthodox Canonical Form)");
    printExpected("Assignment operator message");
    ClapTrap gamma("Gamma");
    printAction("Assigning Beta to Gamma");
    gamma = beta;

    printTestHeader(4, "Self-Assignment Test");
    printExpected("No crash, proper handling");
    printAction("Assigning Gamma to itself");
    ClapTrap& gammaRef = gamma;
    gamma = gammaRef;

    printTestHeader(5, "Basic Attack Test");
    printExpected("Attack message with 0 damage, EP: 10->9");
    printAction("Alpha attacks 'Target1'");
    alpha.attack("Target1");

    printTestHeader(6, "Taking Damage Test");
    printExpected("Damage message, HP: 10->7");
    printAction("Beta takes 3 damage");
    beta.takeDamage(3);

    printTestHeader(7, "Repair Test");
    printExpected("Repair message, HP: 7->12, EP: 10->9");
    printAction("Beta repairs 5 HP");
    beta.beRepaired(5);

    printTestHeader(8, "Chain of Actions");
    printExpected("Multiple attacks and repairs, tracking EP");
    ClapTrap delta("Delta");
    printAction("Attack 3 times, repair 2 times");
    delta.attack("Enemy1");
    delta.attack("Enemy2");
    delta.beRepaired(2);
    delta.attack("Enemy3");
    delta.beRepaired(3);

    printTestHeader(9, "Energy Depletion - Gradual");
    printExpected("EP depletes from 10 to 0, then actions fail");
    ClapTrap epsilon("Epsilon");
    printAction("Performing 5 attacks (EP: 10->5)");
    for (int i = 1; i <= 5; ++i) {
        std::cout << "  Attack #" << i << ": ";
        epsilon.attack("Target");
    }
    printAction("Performing 5 more attacks (EP: 5->0)");
    for (int i = 6; i <= 10; ++i) {
        std::cout << "  Attack #" << i << ": ";
        epsilon.attack("Target");
    }
    printExpected("Next attack should FAIL (EP=0)");
    std::cout << "  Attack #11: ";
    epsilon.attack("ShouldFail");
    printExpected("Repair should FAIL (EP=0)");
    std::cout << "  Repair attempt: ";
    epsilon.beRepaired(5);

    printTestHeader(10, "HP Depletion Test");
    printExpected("HP goes to 0, subsequent actions fail");
    ClapTrap zeta("Zeta");
    printAction("Taking 10 damage (HP: 10->0)");
    zeta.takeDamage(10);
    printExpected("Attack should FAIL (HP=0)");
    zeta.attack("ShouldFail");
    printExpected("Repair should FAIL (HP=0)");
    zeta.beRepaired(5);

    printTestHeader(11, "Overkill Damage Test");
    printExpected("HP doesn't go negative, stays at 0");
    ClapTrap eta("Eta");
    printAction("Taking 1000 damage (HP: 10->0)");
    eta.takeDamage(1000);
    printAction("Attempting actions with 0 HP");
    eta.attack("Target");

    printTestHeader(12, "Edge Case - Zero Damage");
    printExpected("Damage message with 0 damage");
    ClapTrap theta("Theta");
    printAction("Taking 0 damage");
    theta.takeDamage(0);

    printTestHeader(13, "Edge Case - Zero Repair");
    printExpected("Repair message with 0 HP gained, EP still consumed");
    printAction("Repairing 0 HP");
    theta.beRepaired(0);

    printTestHeader(14, "Edge Case - Maximum Values");
    printExpected("Large values handled correctly");
    ClapTrap iota("Iota");
    printAction("Repairing 999 HP");
    iota.beRepaired(999);
    printAction("Taking 1 damage");
    iota.takeDamage(1);

    printTestHeader(15, "Simultaneous HP & EP Depletion");
    printExpected("Both resources at 0");
    ClapTrap kappa("Kappa");
    printAction("Depleting all EP");
    for (int i = 0; i < 10; ++i) {
        kappa.attack("Dummy");
    }
    printAction("Depleting all HP");
    kappa.takeDamage(100);
    printExpected("All actions should FAIL");
    kappa.attack("Fail");
    kappa.beRepaired(5);
    kappa.takeDamage(5);

    printTestHeader(16, "Multiple ClapTraps Interaction Simulation");
    printExpected("Multiple bots attacking each other");
    ClapTrap lambda("Lambda");
    ClapTrap mu("Mu");
    ClapTrap nu("Nu");
    printAction("Lambda attacks Mu");
    lambda.attack("Mu");
    mu.takeDamage(0);
    printAction("Mu attacks Nu");
    mu.attack("Nu");
    nu.takeDamage(0);
    printAction("Nu attacks Lambda");
    nu.attack("Lambda");
    lambda.takeDamage(0);

    printTestHeader(17, "Repair After Damage");
    printExpected("HP should increase correctly");
    ClapTrap xi("Xi");
    printAction("Initial: HP=10");
    printAction("Take 7 damage (HP: 10->3)");
    xi.takeDamage(7);
    printAction("Repair 5 HP (HP: 3->8, EP: 10->9)");
    xi.beRepaired(5);
    printAction("Attack once (EP: 9->8)");
    xi.attack("Target");

    printTestHeader(18, "Copy vs Original Independence");
    printExpected("Changes to copy don't affect original");
    ClapTrap original("Original");
    original.attack("Test");
    ClapTrap copyBot(original);
    printAction("Original attacks (EP should differ)");
    original.attack("Target1");
    printAction("Copy attacks");
    copyBot.attack("Target2");
    printAction("Copy takes damage");
    copyBot.takeDamage(5);

    printTestHeader(19, "Assignment After Modification");
    printExpected("Assigned object has same state as source");
    ClapTrap source("Source");
    source.attack("Test1");
    source.takeDamage(3);
    source.beRepaired(2);
    ClapTrap dest("Destination");
    dest.attack("Test2");
    printAction("Assigning modified Source to Destination");
    dest = source;
    printAction("Destination attacks (state should be from Source)");
    dest.attack("Target");

    printTestHeader(20, "Stress Test - Rapid Actions");
    printExpected("All actions within EP limit succeed");
    ClapTrap omega("Omega");
    printAction("Alternating attacks and repairs");
    omega.attack("T1");
    omega.beRepaired(1);
    omega.attack("T2");
    omega.beRepaired(1);
    omega.attack("T3");
    omega.beRepaired(1);
    omega.attack("T4");
    omega.beRepaired(1);
    omega.attack("T5");
    printExpected("EP should be at 1");
    printAction("One more attack (EP: 1->0)");
    omega.attack("Final");
    printExpected("Next action should FAIL");
    omega.beRepaired(1);

    printSeparator("DESTRUCTION PHASE");
    std::cout << "All ClapTraps will now be destroyed in reverse order of creation." << std::endl;
    std::cout << "Watch for proper destructor messages..." << std::endl;

    return 0;
}
