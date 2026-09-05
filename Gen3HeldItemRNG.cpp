#include <iostream>
#include <cstdint>

using namespace std;

uint8_t grassECSlots[12] = {20, 40, 50, 60, 70, 80, 85, 90, 94, 98, 99, 100};  // Grass
uint8_t water0ECSlots[2] = {70, 100};  // Old Rod
uint8_t water1ECSlots[3] = {60, 80, 100};  // Good Rod
uint8_t water2ECSlots[5] = {40, 80, 95, 99, 100};  //Super Rod
uint8_t water3ECSlots[5] = {60, 90, 95, 99, 100};  // Surf/RockSmash

class PokeRNG {
	private:
		uint32_t seed;
		unsigned long advances;
	public:
		PokeRNG(uint32_t seed) {
			this->seed = seed;
			this->advances = 0;
		}

		uint32_t getSeed() {
			return this->seed;
		}

		unsigned long getAdvances() {
			return this->advances;
		}

		uint32_t next() {
			this->seed = (this->seed * 0x41C64E6D) + 0x6073;
			this->advances++;

			return this->seed;
		}

		uint16_t nextUShort() {
			return next() >> 16;
		}

		void advance(unsigned long n) {
			for (unsigned long i = 0; i < n; i++) {
				next();
			}
		}
};

short getECNumber(short ec) {
	switch (ec) {
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
    case 4:
        return 5;
    default:
        return 12;
	}
}

uint8_t slotLimit(short ec, short index) {
	switch (ec) {
    case 1:
        return water0ECSlots[index];
    case 2:
        return water1ECSlots[index];
    case 3:
        return water2ECSlots[index];
    case 4:
        return water3ECSlots[index];
    default:
        return grassECSlots[index];
	}
}

short calcSlot(short ecType, uint16_t highSeed) {
	uint8_t compare = highSeed % 100;
	short ecNumber = getECNumber(ecType);

	for (short i = 0; i < ecNumber; i++) {
		if (compare < slotLimit(ecType, i)) {
			return i;
		}
	}

	return 255;
}

uint8_t calcNature(uint16_t highSeed) {
	return (highSeed % 25);
}

bool heldItemCheck(uint32_t seed, short percentage) {
	short checkSeedPercentage = (seed >> 16) % 100;

	if (checkSeedPercentage > 44) {
		if ((percentage == 50 && checkSeedPercentage < 95) || (percentage == 5 && checkSeedPercentage > 94)) {
			return true;
		}
	}

	return false;
}

int main() {
	while (true) {
		uint16_t initialSeed;
		cout << "Initial seed: ";
		cin >> hex >> initialSeed;

		unsigned long advances;
		cout << "Current advances: ";
		cin >> dec >> advances;

		short slotType;
		cout << "Slot type (0 Grass, 1 Old Rod, 2 Good Rod, 3 Super Rod, 4 Surf/Rocksmash): ";
		cin >> dec >> slotType;

		short targetSlot;
		cout << "Target slot (0-11 Grass, 0-1 Old Rod, 0-2 Good Rod, 0-4 Super Rod, 0-4 Surf/Rocksmash): ";
		cin >> dec >> targetSlot;

		short method;
		cout << "Target method (1 Method1, 2 Method2, 4 Method4): ";
		cin >> dec >> method;

		short wild;
		cout << "Wild battle delay: ";
		cin >> dec >> wild;

		short delay;
		cout << "Held item delay: ";
		cin >> dec >> delay;

		short percentage;
		cout << "Held item percentage (5/50): ";
		cin >> dec >> percentage;

        uint32_t maxAdvances;
        cout << "Max Advances: ";
        cin >> dec >> maxAdvances;
        cout << endl;

		PokeRNG rng(initialSeed);
		rng.advance(advances);

		while (rng.getAdvances() < maxAdvances) {
			short slot = -1;
			bool heldItem = false;
			uint32_t pid;

			while (slot != targetSlot || !heldItem) {
				PokeRNG go(rng.getSeed());
				go.advance(wild);  // Animation delay

				slot = calcSlot(slotType, go.nextUShort());  // Slot call

				if (slot != targetSlot) {
					rng.next();
					continue;
				}

				go.next();  // Level call
				uint8_t nature = calcNature(go.nextUShort());  // Nature call
				do
				{
					uint16_t low = go.nextUShort();
					uint16_t high = go.nextUShort();
					pid = (high << 16) | low;
				} while (pid % 25 != nature);

				if (method == 2) {
					go.next();
				}

				go.next();  // IV1 call

				if (method == 4) {
					go.next();
				}

				go.next();  // IV2 call

				go.advance(delay);
				
				heldItem = heldItemCheck(go.getSeed(), percentage);

				if (!heldItem) {
					rng.next();
				}
			}

			cout << "Advances: " << uppercase << dec << rng.getAdvances() << "\nPID: " << hex << pid << "\n\n------------------------------------------------------------------------------------------------\n\n";
			rng.next();
		}
	}
}
