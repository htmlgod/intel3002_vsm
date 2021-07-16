#include "StdAfx.h"
#include "DsimModel.h"

UINT8 GetLBit(const UINT8& byte) {
	return byte & 0b01;
}

UINT8 GetHBit(const UINT8& byte) {
	return ((byte & 0b10) >> 1);
}

UINT8 xnor(const UINT8& byte1, const UINT8& byte2) {
	return (~byte1 & ~byte2) || (byte1 & byte2);
}

VOID DsimModel::Ensure()
{
	for (int i = 0; i < 13; i++) {
		MEMORY[i] &= 0b11;
	}
}

VOID DsimModel::ResetCPU(ABSTIME time) {
	// clear memory
	for (int i = 0; i < 13; i++) {
		MEMORY[i] = 0b00;
	}
	pin_X->SetLow;
	pin_Y->SetLow;

	// all tristate outputs to High
	pin_RO->SetHigh;
	pin_CO->SetHigh;
	pin_A0->SetHigh;
	pin_A1->SetHigh;
	pin_D0->SetHigh;
	pin_D1->SetHigh;
}

VOID DsimModel::Propogate(UINT8 vala, UINT8 valb){
	bool a1, a0, b1, b0;
	a1 = vala & 0b10;
	a0 = vala & 0b01;
	b1 = valb & 0b10;
	b0 = valb & 0b01;

	X = a1 & b1 | a0 & b0;
	Y = a1 & b1 | ((a1 | b1) & (a0 | b0));
}

VOID DsimModel::ComputeCarryOut(UINT8 val)
{
	CO = ((val & 0b100) >> 2);
}

UINT8 DsimModel::GetI() {
	INT8 val = 0;

	if (pin_I1->isactive()) {
		val |= (1 << 1);
	}
	if (pin_I0->isactive()) {
		val |= 1;
	}

	return val;
	
}

UINT8 DsimModel::GetCI() {
	UINT8 val = 0;
	val = pin_CI->isactive();
	return val;
}
UINT8 DsimModel::GetK() {
	UINT8 val = 0;

	if (pin_K1->isactive()) {
		val |= (1 << 1);
	}
	if (pin_K0->isactive()) {
		val |= 1;
	}

	return val;
}
UINT8 DsimModel::GetM() {
	UINT8 val = 0;

	if (pin_M1->isactive()) {
		val |= (1 << 1);
	}
	if (pin_M0->isactive()) {
		val |= 1;
	}

	return val;
}


VOID DsimModel::GetAddress()
{
	UINT8 val = 0b0000;

	if (ishigh(pin_F[0]->istate()))
		val |= 0b0001;
	if (ishigh(pin_F[1]->istate()))
		val |= 0b0010;
	if (ishigh(pin_F[2]->istate()))
		val |= 0b0100;
	if (ishigh(pin_F[3]->istate()))
		val |= 0b1000;

	Address = val;
}

VOID DsimModel::DecodeFGroup()
{
	UINT8 val = 0b000;

	if (ishigh(pin_F[4]->istate()))
		val |= 0b001;
	if (ishigh(pin_F[5]->istate()))
		val |= 0b010;
	if (ishigh(pin_F[6]->istate()))
		val |= 0b100;

	Fgroup = val;
}

VOID DsimModel::DecodeRGroup()
{
	if ((Address >= 0b0000 && Address < 0b1010) || (Address == 0b1100 || Address == 0b1101)) {
		Rgroup = 1;
		if (Address == 0b1100)
			Address = T;
		else if (Address == 0b1101)
			Address = AC;
	}
	else if (Address == 0b1010 || Address == 0b1011)
		Rgroup = 2;
	else {
		Rgroup = 3;
		if (Address == 0b1110)
			Address = T;
		else
			Address = AC;
	}	
}
/*
* Ñ F0 ïî F3 òû äîëæåí çàïóñêàòü ôóíêöèþ ComputeCarryOut äëÿ ðåãèñòðà, ãäå ïðîèñõîäèëà îïåðàöèÿ
* 
* À òàêæå Propogate äëÿ X è Y â F0 ïî F3 (ÎÑÒÀËÜÍÛÅ ÕÇ)
*
* Äåøèôðàòîðû êîððåêòíû, åñëè íàïèñàíî AT - òî ïðîñòî MEMORY[Address]
*
* AC - MEMORY[AC], T - MEMORY[T] ÄËß Ð ÃÐÓÏÏÛ 1, Rn - MEMORY[Address], ÄËß 2 È 3 ÑÌÎÒÐÈ ÂÅÐÕÍÅÅ ÏÐÅÄËÎÆÅÍÈÅ
*
* ÄËß CI ÂÛÇÛÂÀÉ GetCI()
* XOR ÝÒÎ ^
* Ñäåëàé òîëüêî f0 è f3, â îñòàëüíûõ ïîôèêñè îøèáêó, ÷òîá êîìïèëÿëîñü
* 
* 
* 
* 
*
*/
VOID DsimModel::ExecuteF0(void) {
	switch (Rgroup)
	{
	case 1: {
		UINT8 val = MEMORY[Address] + (MEMORY[AC] & GetK()) + GetCI();
		ComputeCarryOut(val);
		MEMORY[AC] = val;
		MEMORY[Address] = MEMORY[AC];
		break; }
	case 2: {
		UINT8 val = GetM() + (MEMORY[AC] & GetK()) + GetCI();
		ComputeCarryOut(val);
		MEMORY[Address] = val;
		break; }
	case 3:
		RO = GetLBit(MEMORY[AC]) & (GetLBit(GetI()) & GetLBit(GetK()));
		MEMORY[Address] = ((GetCI() || ((GetHBit(GetI()) && GetHBit(GetK()) && GetHBit(MEMORY[Address])))) * BYTE(2)) | ((MEMORY[Address] << 1) >> 1);
		MEMORY[Address] = ((GetLBit(MEMORY[AC]) && (GetLBit(GetI()) && GetLBit(GetK()))) || (GetHBit(MEMORY[Address]) || (GetHBit(GetI()) && GetHBit(GetK())))) * BYTE(1) | ((MEMORY[Address] >> 1) << 1);
		break;
	default:
		break;
	}
	Propogate(X, Y);
}

VOID DsimModel::ExecuteF1(void) {
	switch (Rgroup)
	{
	case 1: {
		
		MEMORY[Address] = MEMORY[Address] + GetK() + GetCI();
		MEMORY[MAR] = GetK() | MEMORY[Address];
		ComputeCarryOut(MEMORY[Address]);

		break; }
	case 2: 
		MEMORY[MAR] = GetK() | GetM();
		MEMORY[Address] = GetM() + GetK() + GetCI();
		break;

	case 3:
		MEMORY[Address] = (~MEMORY[Address] | GetK()) + (MEMORY[Address] & GetK()) + GetCI();
		break;
	default:
		break;
	}
	ComputeCarryOut(MEMORY[Address]);
	Propogate(X, Y);
}

VOID DsimModel::ExecuteF2(void) {
	switch (Rgroup)
	{
	case 1:
		MEMORY[Address] = MEMORY[AC] & GetK() - 1 + GetCI();
		break;
	case 2:
		MEMORY[Address] = MEMORY[AC] & GetK() - 1 + GetCI();
		break;

	case 3:
		MEMORY[Address] = GetI() & GetK() - 1 + GetCI();
		break;
	default:
		break;
	}
	ComputeCarryOut(MEMORY[Address]);
	Propogate(X, Y);
}

VOID DsimModel::ExecuteF3(void) {
	switch (Rgroup)
	{
	case 1:
		MEMORY[Address] = MEMORY[AC] & GetK() + MEMORY[Address] + GetCI();
		break;
	case 2:
		MEMORY[Address] = MEMORY[AC] & GetK() + GetM() + GetCI();
		break;

	case 3:
		MEMORY[Address] = GetI() & GetK() + MEMORY[Address] + GetCI();
		break;
	default:
		break;
	}
	ComputeCarryOut(MEMORY[Address]);
	Propogate(X, Y);
}

VOID DsimModel::ExecuteF4(void) {
	switch (Rgroup)
	{
	case 1:
		CO = (GetCI() | (MEMORY[Address] & MEMORY[AC] & GetK()));
		MEMORY[Address] = MEMORY[AC] & GetK() & MEMORY[Address];
		break;
	case 2:
		CO = (GetCI() | (GetM() & MEMORY[AC] & GetK()));
		if (Address == 0b1011) {
			MEMORY[AC] = MEMORY[AC] & GetK() & GetM();
		}
		else {
			MEMORY[T] = MEMORY[AC] & GetK() & GetM();
		}
		break;

	case 3:
		if (Address == 0b1111) {
			CO = (GetCI() | (MEMORY[AC] & GetI() & GetK()));
			MEMORY[AC] = GetI() & GetK() & MEMORY[AC];
		}
		else {
			CO = (GetCI() | (MEMORY[T] & GetI() & GetK()));
			MEMORY[T] = GetI() & GetK() & MEMORY[T];
		}
		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF5(void) {
	switch (Rgroup)
	{
	case 1:
		CO = (GetCI() | (MEMORY[Address] & GetK()));
		MEMORY[Address] = GetK() & MEMORY[Address];
		
		break;
	case 2:
		CO = (GetCI() | (GetM() & GetK()));
		if (Address == 0b1011) {
			MEMORY[AC] = GetK() & GetM();
		}
		else {
			MEMORY[T] = GetK() & GetM();
		}
		break;

	case 3:
		if (Address == 0b1111) {
			CO = (GetCI() | (MEMORY[AC] & GetK()));
			MEMORY[AC] = GetK() & MEMORY[AC];
		}
		else {
			CO = (GetCI() | (MEMORY[T] & GetK()));
			MEMORY[T] = GetK() & MEMORY[T];
		}
		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF6(void) {
	switch (Rgroup)
	{
	case 1:
		CO = (islow(pin_CI->istate()) ? 1 : 0) | (MEMORY[AC] & GetK());
		MEMORY[Address] = MEMORY[AC] & GetK() & MEMORY[Address];
		break;
	case 2:
		CO = (islow(pin_CI->istate()) ? 1 : 0) | (MEMORY[AC] & GetK());
		if (Address == 0b1011) {
			MEMORY[AC] = MEMORY[AC] & GetK() & GetM();
		}
		else {
			MEMORY[T] = MEMORY[AC] & GetK() & GetM();
		}
		break;

	case 3:
		CO = (islow(pin_CI->istate()) ? 1 : 0) | (GetI() & GetK());
		if (Address == 0b1111) {
			MEMORY[AC] = GetI() & GetK() & MEMORY[AC];
		}
		else {
			MEMORY[T] = GetI() & GetK() & MEMORY[T];
		}
		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF7(void) {
	switch (Rgroup)
	{
	case 1:
		CO = (islow(pin_CI->istate()) ? 1 : 0) | (MEMORY[Address] & MEMORY[AC] & GetK());
		MEMORY[Address] = xnor(MEMORY[AC] & GetK(), MEMORY[Address]);
		break;
	case 2:
		CO = (islow(pin_CI->istate()) ? 1 : 0) | (GetM() & MEMORY[AC] & GetK());
		if (Address == 0b1011) {
			MEMORY[AC] = xnor(MEMORY[AC] & GetK(), GetM());
		}
		else {
			MEMORY[T] = xnor(MEMORY[AC] & GetK(), GetM());
		}
		break;

	case 3:
		if (Address == 0b1111) {
			CO = (islow(pin_CI->istate()) ? 1 : 0) | (MEMORY[AC] & GetI() & GetK());
			MEMORY[AC] = xnor(GetI() & GetK(), MEMORY[AC]);
		}
		else {
			CO = (islow(pin_CI->istate()) ? 1 : 0) | (MEMORY[T] & GetI() & GetK());
			MEMORY[T] = xnor(GetI() & GetK(), MEMORY[T]);
		}
		break;
	default:
		break;
	}
}

VOID DsimModel::Execute(void) {
	switch (Fgroup) {
	case 0:
		ExecuteF0();
		break;
	case 1:
		ExecuteF1();
		break;
	case 2:
		ExecuteF2();
		break;
	case 3:
		ExecuteF3();
		break;
	case 4:
		ExecuteF4();
		break;
	case 5:
		ExecuteF5();
		break;
	case 6:
		ExecuteF6();
		break;
	case 7:
		ExecuteF7();
	default:
		break;
	}
}

VOID DsimModel::setup(IINSTANCE* instance, IDSIMCKT* dsimckt) {
	char s[8];
	
	inst = instance;
	ckt = dsimckt;

	pin_CI = inst->getdsimpin("CI", true);
	pin_RO = inst->getdsimpin("RO", true);
	pin_CLK = inst->getdsimpin("CLK", true);
	pin_EA = inst->getdsimpin("EA", true);
	pin_ED = inst->getdsimpin("ED", true);
	pin_RI = inst->getdsimpin("RI", true);
	pin_CO = inst->getdsimpin("CO", true);
	pin_X = inst->getdsimpin("X", true);
	pin_Y = inst->getdsimpin("Y", true);

	pin_A0 = inst->getdsimpin("A0", true);
	pin_A1 = inst->getdsimpin("A1", true);

	pin_D0 = inst->getdsimpin("D0", true);
	pin_D1 = inst->getdsimpin("D1", true);
	
	pin_K0 = inst->getdsimpin("K0", true);
	pin_K1 = inst->getdsimpin("K1", true);

	pin_M0 = inst->getdsimpin("M0", true);
	pin_M1 = inst->getdsimpin("M1", true);

	pin_I0 = inst->getdsimpin("I0", true);
	pin_I1 = inst->getdsimpin("I1", true);

	for (int i = 0; i < 7; i++) {
		s[0] = 'F';
		_itoa_s(i, &s[1], 7, 10);
		pin_F[i] = inst->getdsimpin(s, true);
	}

	pin_CI->invert();
	pin_M0->invert();
	pin_M1->invert();
	pin_I0->invert();
	pin_I1->invert();
	pin_K0->invert();
	pin_K1->invert();
	pin_EA->invert();
	pin_ED->invert();
	pin_RI->invert();

	pin_RO->setstates(SHI, SLO, FLT);
	pin_CO->setstates(SHI, SLO, FLT);
	pin_A0->setstates(SHI, SLO, FLT);
	pin_A1->setstates(SHI, SLO, FLT);
	pin_D0->setstates(SHI, SLO, FLT);
	pin_D1->setstates(SHI, SLO, FLT);
	
	pin_RO->invert();
	pin_CO->invert();
	pin_A0->invert();
	pin_A1->invert();
	pin_D0->invert();
	pin_D1->invert();

	// Connects function to handle Clock steps (instead of using "simulate")
	pin_CLK->sethandler(this, (PINHANDLERFN)&DsimModel::clockstep);
	ResetCPU(0);
}

VOID DsimModel::clockstep(ABSTIME time, DSIMMODES mode) {

	if (pin_CLK->isposedge()) {
		GetAddress();
		DecodeFGroup();
		DecodeRGroup();

		Execute();
		Ensure();

		if (CO != -1) {
			if (CO) {
				pin_CO->SetLow;
			}
			else {
				pin_CO->SetHigh;
			}
		}
		else {
			pin_CO->drivetristate(time);
		}
		if (RO != -1) {
			if (RO) {
				pin_RO->SetLow;
			}
			else {
				pin_RO->SetHigh;
			}
		}
		else {
			pin_RO->drivetristate(time);
		}


		pin_X->drivebool(time, X);
		pin_Y->drivebool(time, Y);

		if (islow(pin_EA->istate())) {
			if (MEMORY[MAR] & 0b10) {
				pin_A1->SetLow;
			}
			else {
				pin_A1->SetHigh;
			}
			if (MEMORY[MAR] & 0b01) {
				pin_A0->SetLow;
			}
			else {
				pin_A0->SetHigh;
			}
		}
		else {
			pin_A1->SetFloat;
			pin_A0->SetFloat;
		}

		if (islow(pin_ED->istate())) {
			if (MEMORY[AC] & 0b10) {
				pin_D1->SetLow;
			}
			else {
				pin_D1->SetHigh;
			}
			if (MEMORY[AC] & 0b01) {
				pin_D0->SetLow;
			}
			else {
				pin_D0->SetHigh;
			}
		}
		else {
			pin_D1->SetFloat;
			pin_D0->SetFloat;
		}
	}
	CO = -1;
	RO = -1;
}
INT DsimModel::isdigital(CHAR* pinname)
{
	return TRUE;
}
BOOL DsimModel::indicate(REALTIME time, ACTIVEDATA* data) {
	return FALSE;
}
VOID DsimModel::runctrl(RUNMODES mode) {}
VOID DsimModel::actuate(REALTIME time, ACTIVESTATE newstate) {}
VOID DsimModel::simulate(ABSTIME time, DSIMMODES mode) {}
VOID DsimModel::callback(ABSTIME time, EVENTID eventid) {}