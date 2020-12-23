#include <iostream>
#include "Utils.h"
#include <filesystem>

#pragma region BitDefines
#define FI 0	//done
#define J 1		//done
#define CO 2	//done
#define CE 3	//done
#define OI 4	//done
#define BI 5	//done
#define SU 6	//done
#define EO 7	//done
#define AO 8	//done
#define AI 9	//done
#define II 10	//done
#define IO 11	//done
#define RO 12	//done
#define RI 13	//done
#define MI 14	//done
#define HLT 15	//done
#pragma endregion

bool BitTest(int a, int i) {
	return (a >> i) & 1;
}

struct State {
	uint8_t A=0, B=0, MemoryAddress=0, Step=0, Instruction=0, ProgramCounter=0, Memory[16];
	bool c=false, z=false, hlt=false;
	uint16_t MicroCode[1<<10];
	void ExecuteStep() {

		uint16_t MCAddress = Step | ((uint16_t)(Instruction & 0b11110000) >>1) | ((uint16_t)c << 8) | ((uint16_t)z << 9);
		uint16_t MicroOp = MicroCode[MCAddress];

		ProgramCounter += BitTest(MicroOp, CE);
		if(BitTest(MicroOp, HLT))
			hlt = true;

		uint8_t Bus = 0;
		if (BitTest(MicroOp, CO)) {
			Bus = ProgramCounter;
		}
		else if (BitTest(MicroOp, EO)) {
			uint16_t temp = B;
			if (BitTest(MicroOp, SU)) {
				B = -B;
			}
			Bus = A + B;
			if (BitTest(MicroOp, FI)) {
				z = Bus == 0;
				c = ((uint16_t)A + (uint16_t)B) > 255;
			}
		}
		else if (BitTest(MicroOp, AO)) {
			Bus = A;
		}
		else if (BitTest(MicroOp, IO)) {
			Bus = Instruction &0b1111;
		}
		else if (BitTest(MicroOp, RO)) {
			Bus = Memory[MemoryAddress];
		}

		if (BitTest(MicroOp, J)) {
			ProgramCounter = Bus;
		}
		if (BitTest(MicroOp, OI)) {
			std::cout <<(int) Bus << std::endl;
		}
		if (BitTest(MicroOp, BI)) {
			B = Bus;
		}
		if (BitTest(MicroOp, AI)) {
			A = Bus;
		}
		if (BitTest(MicroOp, II)) {
			Instruction = Bus;
		}
		if (BitTest(MicroOp, RI)) {
			Memory[MemoryAddress] = Bus;
		}
		if (BitTest(MicroOp, MI)) {
			MemoryAddress = Bus;
		}
		Step++;
		Step %= 5;
	}
};
namespace fs = std::filesystem;
int main() {
	State s;
	//load contents from files
	{

		char* contents;
		char* contentsb;
		auto directory = OpenFileName();
		size_t size = ReadFile(directory, contents);
		directory[directory.size() - 5] = L'1';
		size = ReadFile(directory, contentsb);

		uint64_t* A = (uint64_t*)contents;
		uint64_t* B = (uint64_t*)contentsb;
		for (int i = 0; i < (1 << 10); i++) {
			s.MicroCode[i] = (A[i]) | (B[i] << 8);
		}
		char* memory;
		auto memdir = OpenFileName();
		ReadFile(memdir, memory);
		for (int i = 0; i < 16; i++) {
			s.Memory[i] = memory[i];
		}
	}
	while (!s.hlt) {
		s.ExecuteStep();
	}
}