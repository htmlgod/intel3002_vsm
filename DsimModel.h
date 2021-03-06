#pragma once
#include "StdAfx.h"
#include "sdk/vsm.hpp"
#include <vector>

#define SetHigh setstate(time, 1, SHI)
#define SetLow setstate(time, 1, SLO)
#define SetFloat setstate(time, 1, FLT)

// REGISTERS ADDRESS CONSTANTS
#define T 10
#define AC 11
#define MAR 12

class DsimModel : public IDSIMMODEL
{
public:
	INT isdigital(CHAR* pinname);
	VOID setup(IINSTANCE* inst, IDSIMCKT* dsim);
	VOID runctrl(RUNMODES mode);
	VOID actuate(REALTIME time, ACTIVESTATE newstate);
	BOOL indicate(REALTIME time, ACTIVEDATA* data);
	VOID clockstep(ABSTIME time, DSIMMODES mode);
	VOID simulate(ABSTIME time, DSIMMODES mode);
	VOID callback(ABSTIME time, EVENTID eventid);
private:
	VOID ResetCPU(ABSTIME time);
	VOID Execute(void);
	VOID ExecuteF0(void);
	VOID ExecuteF1(void);
	VOID ExecuteF2(void);
	VOID ExecuteF3(void);
	VOID ExecuteF4(void);
	VOID ExecuteF5(void);
	VOID ExecuteF6(void);
	VOID ExecuteF7(void);

	VOID Propogate(UINT8 vala, UINT8 valb);
	VOID ComputeCarryOut(UINT8 val);

	VOID GetAddress();
	VOID DecodeFGroup();
	VOID DecodeRGroup();

	VOID Ensure();


	UINT8 GetI();
	UINT8 GetK();
	UINT8 GetM();
	UINT8 GetCI();


	IINSTANCE* inst;
	IDSIMCKT* ckt;
	
	// inputs
	// inverted
	IDSIMPIN* pin_CI; // carry in
	IDSIMPIN* pin_M0; // input
	IDSIMPIN* pin_M1; // input
	IDSIMPIN* pin_I0; // external bus input
	IDSIMPIN* pin_I1; // external bus input
	IDSIMPIN* pin_K0; // mask address input
	IDSIMPIN* pin_K1; // mask address input
	IDSIMPIN* pin_EA; // address enable input
	IDSIMPIN* pin_ED; // data enable input
	IDSIMPIN* pin_RI; // shift right input

	// default
	IDSIMPIN* pin_CLK; // sync
	IDSIMPIN* pin_F[7]; // microoperations input
	// r group    |F GROUP
	// F0,F1,F2,F3,F4,F5,F6


	// outputs
	IDSIMPIN* pin_X; // propogate output
	IDSIMPIN* pin_Y; // propogate output
	//inverted
	IDSIMPIN* pin_RO; // shift right output (TRISTATE)
	IDSIMPIN* pin_CO; // carry output (TRISTATE)
	IDSIMPIN* pin_A0; // adress output (TRISTATE)
	IDSIMPIN* pin_A1; // adress output (TRISTATE)
	IDSIMPIN* pin_D0; // data output (TRISTATE)
	IDSIMPIN* pin_D1; // data output (TRISTATE)

	// r1, ...,  r9, T, AC, MAR
	BYTE MEMORY[13];

	UINT8 Rgroup;
	UINT8 Fgroup;
	UINT8 Address;

	INT RO = -1;
	INT CO = -1;

	BOOL X;
	BOOL Y;
};
