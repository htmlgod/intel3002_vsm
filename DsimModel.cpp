#include "StdAfx.h"
#include "DsimModel.h"

VOID DsimModel::ResetCPU(ABSTIME time) {
	// clear memory
	intel3002_reg.MAR = 0b00;
	intel3002_reg.AC = 0b00;
	intel3002_reg.T = 0b00;
	for (int i = 0; i < 10; i++) {
		intel3002_reg.REG[i] = 0b00;
	}
	// all outputs to HIGH
	pin_RO->SetHigh;
	pin_CO->SetHigh;
	pin_X->SetHigh;
	pin_Y->SetHigh;
	pin_A0->SetHigh;
	pin_A1->SetHigh;
	pin_D0->SetHigh;
	pin_D1->SetHigh;
}

VOID DsimModel::Propogate(ABSTIME time, UINT8 vala, UINT8 valb){
	UINT8 a1, a0, b1, b0;
	a1 = vala & 0b10;
	a0 = vala & 0b01;
	b1 = valb & 0b10;
	b0 = valb & 0b01;

	if (a1 & b1 | a0 & b0) {
		pin_X->SetHigh;
	}
	else {
		pin_X->SetLow;
	}
	if (a1 & b1 | ((a1 | b1) & (a0 | b0))) {
		pin_Y->SetHigh;
	}
	else {
		pin_Y->SetLow;
	}
}
UINT8 DsimModel::GetI() {
	UINT8 val = 0;

	
}
UINT8 DsimModel::GetK() {
	UINT8 val = 0;

	if (ishigh(pin_K1->istate())) {
		val |= (1 << 1);
	}
	if (ishigh(pin_K0->istate())) {
		val |= 1;
	}

	return val;
}
UINT8 DsimModel::GetM() {

}


VOID DsimModel::DecodeAddress()
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
	if ((Address >= 0b0000 && Address < 0b1010) || (Address == 0b1100 || Address == 0b1101))
		Rgroup = 1;
	else if (Address == 0b1010 || Address == 0b1011)
		Rgroup = 2;
	else 
		Rgroup = 3;
}

VOID DsimModel::ExecuteF0(void) {
	switch (Rgroup)
	{
	case 1:
		intel3002_reg.AC = intel3002_reg.REG[Address] + (intel3002_reg.AC & GetK()) + (islow(pin_CI->istate()) ? 1 : 0);
		intel3002_reg.REG[Address] = intel3002_reg.AC;
		
		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF1(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF2(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF3(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF4(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF5(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

		break;
	default:
		break;
	}
}

VOID DsimModel::ExecuteF6(void) {
	switch (Rgroup)
	{
	case 1:

		break;
	case 2:

		break;

	case 3:

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

	// Connects function to handle Clock steps (instead of using "simulate")
	pin_CLK->sethandler(this, (PINHANDLERFN)&DsimModel::clockstep);

	ResetCPU(0);
}

VOID DsimModel::clockstep(ABSTIME time, DSIMMODES mode) {

	if (pin_CLK->isnegedge()) {
		DecodeAddress();
		DecodeFGroup();
		DecodeRGroup();

		Execute();




		if (islow(pin_EA->istate())) {
			if (intel3002_reg.MAR & 0b10) {
				pin_A1->SetHigh;
			}
			else {
				pin_A1->SetLow;
			}
			if (intel3002_reg.MAR & 0b01) {
				pin_A0->SetHigh;
			}
			else {
				pin_A0->SetLow;
			}
		}
		else {
			pin_A1->SetFloat;
			pin_A0->SetFloat;
		}

		if (islow(pin_ED->istate())) {
			if (intel3002_reg.AC & 0b10) {
				pin_D1->SetHigh;
			}
			else {
				pin_D1->SetLow;
			}
			if (intel3002_reg.AC & 0b01) {
				pin_D0->SetHigh;
			}
			else {
				pin_D0->SetLow;
			}
		}
		else {
			pin_D1->SetFloat;
			pin_D0->SetFloat;
		}
	}
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