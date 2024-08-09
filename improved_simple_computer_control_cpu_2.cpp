/***************************************************************************************
                System Components and Control Operations

    Control Operations                      Explanation
 -------------------------------------------------------------------------------------
 1. GPR -> M                [GPR_M]       // Write contents of GPR into addressed memory location
 2. PC + 1 -> PC            [INCPC]       // Increment PC
 3. GPR(AD) -> PC           [GPR_PC]      // Transfer address bits in general-purpose register to PC
 4. PC —> MAR               [PC_MAR]      // Transfer from PC to MAR
 5. GPR(AD) -> MAR          [GPR_MAR]     // Transfer address bits in general-purpose register to MAR
 6. GPR(OP)-> OPR           [GPR_OPR]     // Transfer operation bits in general-purpose register to OPR
 7. M -> GPR                [M_GPR]       // Transfer addressed word to GPR
 8. Acc -> GPR              [Acc_GPR]     // Transfer contents of Acc to GPR
 9. PC -> GPR(AD)           [PC_GPR]      // Transfer contents of program counter to address part of MAR
 10. GPR + 1 -> GPR         [INCGPR]      // Increment GPR
 11. GPR + Acc -> Acc       [ADD]         // Add number in GPR to number in Acc and leave sum in Acc
 12. 0 -> Acc               [CLRAcc]      // Clear Acc
 13. ROR F, Acc             [ROR]         // Rotate Acc to right through F
 14. ROL F. Acc             [ROL]         // Rotate Acc to left through F
 15. 0 -> F                 [CLRF]        // Reset flip-flop F
 16. ~F -> F                [COMF]        // Complement flip-flop F
 17. ~Acc —> Acc            [COMAcc]      // Complement Acc
 18. Acc + 1 —> Acc         [INCAcc]      // Increment Acc

 ****************************************************************************************/

#include <iostream>
#include <windows.h>
#include <memory>
#include <vector>



// 18 microoperations + 2
// 0b [INCA] [COMA] [COMF] [CLRF] [ROL] [ROR] [CLRA] [ADD] [INCGPR] [PC_GPR] [A_GPR] [M_GPR] [GPR_OP] [GPR_MAR] [PC_MAR]  [GPR_PC] [INCPC] [GPR_M]

#define GPR_M       0b00000000000000000000000000000001		// -1  definition of control lines (DWORD 32-bit)
#define INCPC       0b00000000000000000000000000000010      // -2
#define GPR_PC      0b00000000000000000000000000000100      // -3
#define PC_MAR      0b00000000000000000000000000001000      // -4
#define GPR_MAR     0b00000000000000000000000000010000      // -5
#define GPR_OP      0b00000000000000000000000000100000      // -6
#define M_GPR       0b00000000000000000000000001000000      // -7
#define A_GPR       0b00000000000000000000000010000000      // -8
#define PC_GPR 	    0b00000000000000000000000100000000      // -9
#define INCGPR      0b00000000000000000000001000000000      // -10
#define ADD   		0b00000000000000000000010000000000      // -11
#define CLRA   		0b00000000000000000000100000000000      // -12
#define ROR   		0b00000000000000000001000000000000      // -13
#define ROL   		0b00000000000000000010000000000000      // -14
#define CLRF   		0b00000000000000000100000000000000      // -15
#define COMF   		0b00000000000000001000000000000000      // -16
#define COMA   		0b00000000000000010000000000000000      // -17
#define INCA   		0b00000000000000100000000000000000      // -18
#define INCPCF     	0b00010000000000000000000000000000      // -19
#define INCPCZ     	0b00100000000000000000000000000000      // -20
#define CLRSC      	0b01000000000000000000000000000000      // -21
#define HLT        	0b10000000000000000000000000000000      // -22

#define R_nW1   	0b00000000000001000000000000000000
#define R_nW2   	0b00000000000010000000000000000000
#define R_nW3   	0b00000000000100000000000000000000
#define R_nW4   	0b00000000001000000000000000000000
#define R_nW5   	0b00000000010000000000000000000000
#define R_nW6   	0b00000000100000000000000000000000
#define R_nW7   	0b00000001000000000000000000000000
#define R_nW8   	0b00000010000000000000000000000000
#define R_nW9   	0b00000100000000000000000000000000
#define R_nW10   	0b00001000000000000000000000000000

// typedef unsigned char BYTE;        // 8-bits
// typedef unsigned short int WORD;   // 16-bits
// typedef unsigned int DWORD;        // 32-bits

class Component							// component interface class of the CPU
{
public:
	virtual void Reset() {}				// virtual defines that this Function will be overwritten in derived classes
	virtual void RisingEdge() {}        // Control BUS
	virtual void HighLevel() {}         // for output ( Component -> BUS )
	virtual void FallingEdge() {}       // increment, complement, swap, clear vb.
	virtual void LowLevel() {}          // for input   ( BUS -> Component )

};


class Register : public Component
{
private:

    WORD mStore;				// internal storage
    WORD& mDataBus;
    DWORD& mControlBus;
    DWORD mInMask, mOutMask, mIncrMask;
    WORD mBitMask;

public:

    Register(WORD& port, DWORD& ctrl, DWORD inmask, DWORD outmask, DWORD incrmask, WORD bitmask)
    : mDataBus(port), mControlBus(ctrl), mInMask(inmask), mOutMask(outmask), mIncrMask(incrmask), mBitMask(bitmask) {}  // bitmask - kaç bit register ise set eder.


    void HighLevel()        // for output [Component, Memory -> BUS]
    {

		if (mControlBus & mOutMask)
		{
            mDataBus = mStore;
		}

    }

    void FallingEdge()      // // increment, complement, swap, clear vb.
    {
        if ((mControlBus & mIncrMask) || mIncrMask == 0xffff)
        {
            Increment();
        }
    }

	void LowLevel()      // for input [BUS -> Component, Memory]
	{
		if (mControlBus & mInMask)
		{
            mStore = mDataBus & mBitMask;
		}

	}

    void Reset()
    {
        mStore = 0;
    }

    void Set(WORD& data)
    {
        mStore = data & mBitMask;
    }

    WORD Get()
    {
        return mStore;
    }

    void Increment()
    {
        mStore = (mStore + 1) & mBitMask;
    }

 };

 class GprBus: public Component
{
private:

    WORD mStore;				// internal storage
    WORD& mDataBus;
    DWORD& mControlBus;
    DWORD mInMask, mOutMask;
    std::shared_ptr<Register> mPC, mOPR, mGPR;

public:

    GprBus(WORD& port, DWORD& ctrl, DWORD inmask, DWORD outmask, std::shared_ptr<Register> pc, std::shared_ptr<Register> opr, std::shared_ptr<Register> gpr)
	: mDataBus(port), mControlBus(ctrl), mInMask(inmask), mOutMask(outmask), mPC(pc), mOPR(opr), mGPR(gpr) {}


    void HighLevel()        // for output [Component -> BUS]
    {

    }

    void FallingEdge()
    {
        WORD a, b;

        if (mControlBus & mInMask)
		{
            if ((mControlBus & mInMask) == PC_GPR)
            {
                a = mPC->Get() & 0x00ff;
                b = mGPR->Get() & 0x00ff;
                mPC->Set(b);
                mGPR->Set(a);
            }

            if ((mControlBus & mInMask) == GPR_OP)
            {
                a = mGPR->Get() >> 8;
                mOPR->Set(a);
            }

		}
    }

    void LowLevel()      // for input [BUS -> Component]
	{


	}

};

class Adder : public Component
{
private:

    WORD mNextFlag, mFlagZ, mFlagF;
    WORD mTemp;
    WORD mStore;
    WORD& mDataBus;
    DWORD& mControlBus;
    DWORD mInMask, mOutMask;
    std::shared_ptr<Register> mRegAcc, mRegGpr, mRegF;

public:
	Adder(WORD& port, DWORD& ctrl, DWORD inmask, DWORD outmask, std::shared_ptr<Register> acc, std::shared_ptr<Register> gpr, std::shared_ptr<Register> flg)
	: mDataBus(port), mControlBus(ctrl), mInMask(inmask), mOutMask(outmask), mRegAcc(acc), mRegGpr(gpr), mRegF(flg) {}

	void HighLevel()       // for output
    {

		mStore = (mRegAcc->Get() + mRegGpr->Get()) & 0x0fff ;

		if (mControlBus & mOutMask)
		{
			// mDataBus = mRegAcc->Get();
		}
    }

    void FallingEdge()
    {
        WORD a;

        mNextFlag = mRegF->Get();
        (mNextFlag & 0x0001) ?  mFlagF = 1 : mFlagF = 0;        // Save Current Flag;

		if (mRegGpr->Get())           // 0b0000'00ZF
        {
            mNextFlag = mNextFlag & 0x0001; // set Z = 0 bit 1 in Flag Reg. 0b0000'000F
            mRegF->Set(mNextFlag) ;
        }
		else
        {
            mNextFlag = mNextFlag | 0x0002;   // set Z = 0 bit 1 in Flag Reg. 0b0000'001F
            mRegF->Set(mNextFlag) ;         // if GPR = 0 => Z = 1
        }

        if (mControlBus & mInMask)
		{
            if ((mControlBus & mInMask) == A_GPR)
            {
                a = (mRegAcc->Get() & 0x0fff);
                mRegGpr->Set(a);
            }

            if ((mControlBus & mInMask) == ADD)
            {
                mRegAcc->Set(mStore);
            }

            if ((mControlBus & mInMask) == COMA)
            {
                a = (~(mRegAcc->Get()) & 0x0fff);   // a = ((mRegAcc->Get() ^ 0xffff) & 0x0fff);
                mRegAcc->Set(a);
            }

            if ((mControlBus & mInMask) == COMF)
            {
                a = (((mRegF->Get()) ^ 0x0001) & 0x0003);
                mRegF->Set(a);
            }

            if ((mControlBus & mInMask) == CLRA)
            {
                mRegAcc->Reset();
            }

            if ((mControlBus & mInMask) == CLRF)
            {
                a = ((mRegF->Get()) & 0x0002);
                mRegF->Set(a);
            }

            if ((mControlBus & mInMask) == ROL)
            {
                RolAcc();
            }

            if ((mControlBus & mInMask) == ROR)
            {
                RorAcc();
            }

		}

    }

    void RolAcc()
    {
        mStore = mRegAcc->Get();
        if (mStore & 0x0800)
        {
            mNextFlag |= 0x0001;
            mRegF->Set(mNextFlag);
        }
        else
        {
            mNextFlag &= 0x0002;
            mRegF->Set(mNextFlag);  // if msb =1 F flag is 1 else 0.
        }
        mStore = ((mStore << 1) & 0x0fff) | mFlagF;
        mRegAcc->Set(mStore);
    }

    void RorAcc()
    {
        WORD acc;
        acc = mRegAcc->Get();
        if (acc & 0x0001)
        {
            mNextFlag |= 0x0001;
            mRegF->Set(mNextFlag);
        }
        else
        {
            mNextFlag &= 0x0002;
            mRegF->Set(mNextFlag);  // if lsb =1 F flag is 1 else 0.
        }
        acc = ((acc >> 1) & 0x0fff);
        mFlagF ? (acc |= 0x0800) : (acc |= 0x0000);
        mRegAcc->Set(acc);
    }

};

class Memory : public Component
{
private:
    WORD  mStore[128];				// internal storage 16 bit (12-bit used)
    WORD& mDataBus;
    DWORD& mControlBus;
    DWORD mInMask, mOutMask;
    std::shared_ptr<Register> mMAR;
    WORD n_size;

public:

    Memory(WORD& port, DWORD& ctrl, DWORD inmask, DWORD outmask, std::shared_ptr<Register> mar)
	: mDataBus(port), mControlBus(ctrl), mInMask(inmask), mOutMask(outmask), mMAR(mar)
	{
/*
            SUMMARY OF INSTRUCTIONS                                    [Opcode]
    ----------------------------------------------------------------------------
     1. CRA clear accumulator   ........................................[0001]
     2. CTA complement accumulator  ....................................[0010]
     3. ITA increment accumulator   ....................................[0011]
     4. CRF clear flip-flop F   ........................................[0100]
     5. CTF complement flip-flop F  ....................................[0101]
     6. SFZ skip next instruction if F = 0  ............................[0110]
     7. ROR rotate right    ............................................[0111]
     8. ROL rotate left     ............................................[1000]
     9. ADD add to accumulator  ........................................[1001]
     10. ADDI add indirect to accumulator   ............................[1010]
     11. STA store in memory from accumulator   ........................[1011]
     12. JMP jump       .........,,,,,,,,...............................[1100]
     13. JMPI jump indirect     ........................................[1101]
     14. CSR call subroutine    ........................................[1110]
     15. ISZ increment and skip if Z = 0    ............................[1111]
     16. HLT Halt       ................................................[0000]
    -----------------------------------------------------------------------------

*/

        WORD prg[] = {
            0b0000'0001'0000'0000, // 00 CRA  x x           CRA
            0b0000'1010'0000'1000, // 01 ADDI 0 8     LOOP  ADDI ANA
            0b0000'1111'0000'1000, // 02 ISZ  0 8           ISZ ANA
            0b0000'1111'0000'1001, // 03 ISZ  0 9           ISZ CTR
            0b0000'1100'0000'0001, // 04 JMP  0 1           JMP LOOP
            0b0000'1011'0000'0111, // 05 STA  0 7           STA RES
            0b0000'0000'0000'0000, // 06 HLT  x x           HLT
            0b0000'0000'0000'0000, // 07 0 0 0  [RES]  <- Store result
            0b0000'0000'0000'1010, // 08 0 0 A  [ANA]
            0b0000'1111'1111'1010, // 09 F F A  [CTR] (-6)
            0b0000'0000'0000'0001, // 10 0 0 1  [00A] (Adding numbers) <- first number
            0b0000'0000'0000'0011, // 11 0 0 3
            0b0000'0000'0000'0101, // 12 0 0 5
            0b0000'0000'0000'0111, // 13 0 0 7
            0b0000'0000'0000'1001, // 14 0 0 9
            0b0000'0000'0000'1011, // 15 0 0 B  <- Last number
            0b0000'0000'0000'0000 };

/* Prog - 2
        WORD prg[] = {
            0b0000'0001'0000'0000, // 00 CRA x x    CRA
            0b0000'1001'0000'0111, // 01 ADD 0 7    ADD SUB
            0b0000'0010'0000'0000, // 02 CTA x x    CTA
            0b0000'0011'0000'0000, // 03 ITA x x    ITA
            0b0000'1001'0000'1000, // 04 ADD 0 8    ADD MIN
            0b0000'1011'0000'1001, // 05 STA 0 9    STA DIF
            0b0000'0000'0000'0000, // 06 HLT x x    HLT
            0b0000'0000'1001'1100, // 07 0 9 C  [SUB]
            0b0000'0000'1011'0111, // 08 0 B 7  [MIN]
            0b0000'0000'0000'0000, // 09 x x x  [DIF]
            0b0000'0000'0000'0000, // 10
            0b0000'0000'0000'0000, // 11
            0b0000'0000'0000'0000, // 12
            0b0000'0000'0000'0000, // 13
            0b0000'0000'0000'0000, // 14
            0b0000'0000'0000'0000, // 15
            0b0000'0000'0000'0000 };

*/

/*  Prog - 1
        WORD prg[] = {
            0b0000'0001'0000'0000, // 00 CRA x x
            0b0000'1001'0000'0110, // 01 ADD 0 6
            0b0000'1001'0000'0111, // 02 ADD 0 7
            0b0000'1001'0000'1000, // 03 ADD 0 8
            0b0000'1011'0000'1001, // 04 STA 0 9
            0b0000'0000'0000'0000, // 05 HLT x x
            0b0000'0000'0001'0111, // 06 0 1 7
            0b0000'0000'0000'1011, // 07 0 0 B
            0b0000'0000'0001'1100, // 08 0 1 C
            0b0000'0000'0000'0000, // 09 x x x
            0b0000'0000'0000'0000, // 10
            0b0000'0000'0000'0000, // 11
            0b0000'0000'0000'0000, // 12
            0b0000'0000'0000'0000, // 13
            0b0000'0000'0000'0000, // 14
            0b0000'0000'0000'0000, // 15
            0b0000'0000'0000'0000 };

*/


        n_size = sizeof(prg)/sizeof(WORD);
        std::cout << "\n****** WORD n_size ****** : " << (int)n_size << "\n";

        for (int i = 0; i < n_size; i++) // load program to memory
        {
            mStore[i] =  prg[i];
            std::cout << mStore[i] << " - " ;
        }
	}

    void HighLevel()
	{
		if (mControlBus & mOutMask)
		{
			mDataBus = mStore[mMAR->Get()];		// RAM -> BUS
		}
	}

	void LowLevel()
	{
	    if (mControlBus & mInMask)
		{
            mStore[mMAR->Get()] = mDataBus;	     // BUS -> RAM																																	// 0x0000-0x7fff: schreiben in RAM
		}
		DisplayMemory();
	}

	void Set(WORD& data)
    {
        mStore[mMAR->Get()] = data;
    }

    WORD Get()
    {
        return mStore[mMAR->Get()];
    }

    void DisplayMemory()
    {
        std::cout << "\n****** WORD n_size ****** : " << (int)n_size << "\n";

        for (int i = 0; i < n_size; i++)
        {
            std::cout << mStore[i] << " - " ;
        }
    }

};



/*******************************************************************************************************************

                            INSTRUCTIONS [Fetch Cycle]
    ------------------------------------------------------------------
     Clock cycle    Microoperation             Explanation
    ------------------------------------------------------------------
        1           PC -> MAR       [PC_MAR]        Transfer instruction location
                                                    from program counter to memory
                                                    address register

        2           M -> GPR        [M_GPR]         Transfer addressed word to
                    PC+1 -> PC      [INCPC]         general-purpose register: increment
                                                    program counter

        3           GPR(OP) -> OPR  [GPR_OP]        Transfer operation part of
                                                    instruction to operation register
    ----------------------------------------------------------------------

     INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic      Explanation                         Microoperation
    ------------------------------------------------------------------------
        CRA        Clear accumulator                    0 -> Acc        [CLRA]
                                                        0 -> SC         [CLRSC]  -  Clear Sequence Counter (2 clock cycle)

        CTA        Complement accumulator               ~Acc -> Acc     [COMA]
                                                        0 -> SC         [CLRSC]

        ITA        Increment accumulator                Acc + 1 -> Acc  [INCA]
                                                        0 -> SC         [CLRSC]

        CRF        Clear flip-flop F                    0 -> F          [CLRF]
                                                        0 -> SC         [CLRSC]

        CTF        Complement flip-flop F               ~F -> F         [COMF]
                                                        0 -> SC         [CLRSC]

        SFZ         Skip next instruction               PC + 1 -> PC    [INCPCF]  (if F = 0)
                        if F is zero                    0 -> SC         [CLRSC]


        ROR         Rotate right through                Rotate right    [ROR]
                        F and Acc                       0 -> SC         [CLRSC]

        ROL         Rotate left through                 Rotate left     [ROL]
                        F and Acc                       0 -> SC         [CLRSC]

        HLT         HALT                                HLT             [HLT]
    ------------------------------------------------------------------------


    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    ADD ,Address    :           1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   M —> GPR            [M_GPR]

                                3                   GPR + Acc -> Acc    [ADD]

                                4                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    ADDI ,Address    :          1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   M —> GPR            [M_GPR]

                                3                   GPR(AD) -> MAR      [GPR_MAR]

                                4                   M —> GPR            [M_GPR]

                                5                   GPR + Acc -> Acc    [ADD]

                                6                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    STA ,Address    :           1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   Acc —> GPR          [A_GPR]

                                3                   GPR -> M            [GPR_M]

                                4                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    JMP ,Address    :           1                   GPR(AD) -> PC       [GPR_PC]

                                2                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    JMPI ,Address    :          1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   M —> GPR            [M_GPR]

                                3                   GPR(AD) -> PC       [GPR_PC]

                                4                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    CSR ,Address    :           1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   GPR(AD) -> PC       [GPR_PC]
                                                    PC -> GPR(AD)       [PC_GPR]

                                3                   GPR(AD) -> M        [GPR_M]

                                4                   PC + 1 -> PC        [INCPC]

                                5                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

    INSTRUCTIONS [Execute Cycle]
    ------------------------------------------------------------------------
     Mnemonic               Clock cycle            Microoperation
    ------------------------------------------------------------------------
    ISZ ,Address    :           1                   GPR(AD) -> MAR      [GPR_MAR]

                                2                   M -> GPR            [M_GPR]

                                3                   GPR + 1 -> GPR      [INCGPR]

                                4                   GPR -> M            [GPR_M]

                                5                   PC + 1 -> PC        [INCPCZ] (if GPR = 0 or Z = 1)

                                6                   0 -> SC             [CLRSC]
    ------------------------------------------------------------------------

*******************************************************************************************************************/

class Control : public Component
{
private:

	BYTE mOpcode;
	BYTE mHltBit;
	BYTE mFlagZ;
    BYTE mFlagF;
	WORD& mDataBus;                     // reference to the IO lines the component is connected to
    DWORD& mControlBus;                  // reference to the control word
	std::shared_ptr<Register> mRegOpr, mRegSteps, mRegFlg;
	DWORD mMicrocode[136];

public:

    Control(WORD& port, DWORD& ctrl, std::shared_ptr<Register> opreg, std::shared_ptr<Register> sreg, std::shared_ptr<Register> flgreg)
	: mDataBus(port), mControlBus(ctrl), mRegOpr(opreg), mRegSteps(sreg), mRegFlg(flgreg)
	{
// 0b[HLT][CLRSC][INCPCZ][INCPCF]0000000000[INCA][COMA][COMF][CLRF][ROL][ROR][CLRA][ADD][INCGPR][PC_GPR][A_GPR][M_GPR][GPR_OP][GPR_MAR][PC_MAR][GPR_PC][INCPC][GPR_M]
        DWORD Microprg[] = {
            0b00000000000000000000000000001000, //0 PC -> MAR               [PC_MAR]       .........................[FETCH]
            0b00000000000000000000000001000010, //1 M -> GPR, PC+1 -> PC    [M_GPR][INCPC]     .....................[FETCH]
            0b00000000000000000000000000100000, //2 GPR(OP) -> OPR          [GPR_OP]       .........................[FETCH]
            0b00000000000000000000000000000000, //3
            0b00000000000000000000000000000000, //4
            0b00000000000000000000000000000000, //5
            0b00000000000000000000000000000000, //6
            0b00000000000000000000000000000000, //7
            0b00000000000000000000100000000000, //8 <0001-CRA>  0 -> Acc    [CLRA]      ............................[EXECUTE]
            0b01000000000000000000000000000000, //9 <0001-CRA>  0 -> SC     [CLRSC]     ............................[EXECUTE]
            0b00000000000000000000000000000000, //10
            0b00000000000000000000000000000000, //11
            0b00000000000000000000000000000000, //12
            0b00000000000000000000000000000000, //13
            0b00000000000000000000000000000000, //14
            0b00000000000000000000000000000000, //15
            0b00000000000000010000000000000000, //16 <0010-CTA>  ~Acc -> Acc [COMA]      ...........................[EXECUTE]
            0b01000000000000000000000000000000, //17 <0010-CTA>  0 -> SC     [CLRSC]     ...........................[EXECUTE]
            0b00000000000000000000000000000000, //18
            0b00000000000000000000000000000000, //19
            0b00000000000000000000000000000000, //20
            0b00000000000000000000000000000000, //21
            0b00000000000000000000000000000000, //22
            0b00000000000000000000000000000000, //23
            0b00000000000000100000000000000000, //24 <0011-ITA>  Acc+1 -> Acc   [INCA]   ............................[EXECUTE]
            0b01000000000000000000000000000000, //25 <0011-ITA>  0 -> SC        [CLRSC]   ...........................[EXECUTE]
            0b00000000000000000000000000000000, //26
            0b00000000000000000000000000000000, //27
            0b00000000000000000000000000000000, //28
            0b00000000000000000000000000000000, //29
            0b00000000000000000000000000000000, //30
            0b00000000000000000000000000000000, //31
            0b00000000000000000100000000000000, //32 <0100-CRF>  0 -> F        [CLRF]   ............................[EXECUTE]
            0b01000000000000000000000000000000, //33 <0100-CRF>  0 -> SC       [CLRSC]   ...........................[EXECUTE]
            0b00000000000000000000000000000000, //34
            0b00000000000000000000000000000000, //35
            0b00000000000000000000000000000000, //36
            0b00000000000000000000000000000000, //37
            0b00000000000000000000000000000000, //38
            0b00000000000000000000000000000000, //39
            0b00000000000000001000000000000000, //40 <0101-CTF>  ~F -> F       [COMF]   ............................[EXECUTE]
            0b01000000000000000000000000000000, //41 <0101-CTF>  0 -> SC       [CLRSC]   ...........................[EXECUTE]
            0b00000000000000000000000000000000, //42
            0b00000000000000000000000000000000, //43
            0b00000000000000000000000000000000, //44
            0b00000000000000000000000000000000, //45
            0b00000000000000000000000000000000, //46
            0b00000000000000000000000000000000, //47
            0b00010000000000000000000000000000, //48 <0110-SFZ>  PC+1 -> PC    [INCPCF]  ...........................[EXECUTE]
            0b01000000000000000000000000000000, //49 <0110-SFZ>  0 -> SC       [CLRSC]   ...........................[EXECUTE]
            0b00000000000000000000000000000000, //50
            0b00000000000000000000000000000000, //51
            0b00000000000000000000000000000000, //52
            0b00000000000000000000000000000000, //53
            0b00000000000000000000000000000000, //54
            0b00000000000000000000000000000000, //55
            0b00000000000000000001000000000000, //56 <0111-ROR>  Rotate right  [ROR]   .............................[EXECUTE]
            0b01000000000000000000000000000000, //57 <0111-ROR>  0 -> SC       [CLRSC]   ...........................[EXECUTE]
            0b00000000000000000000000000000000, //58
            0b00000000000000000000000000000000, //59
            0b00000000000000000000000000000000, //60
            0b00000000000000000000000000000000, //61
            0b00000000000000000000000000000000, //62
            0b00000000000000000000000000000000, //63
            0b00000000000000000010000000000000, //64 <1000-ROL>  Rotate left  [ROL]   ..............................[EXECUTE]
            0b01000000000000000000000000000000, //65 <1000-ROR>  0 -> SC      [CLRSC]   ............................[EXECUTE]
            0b00000000000000000000000000000000, //66
            0b00000000000000000000000000000000, //67
            0b00000000000000000000000000000000, //68
            0b00000000000000000000000000000000, //69
            0b00000000000000000000000000000000, //70
            0b00000000000000000000000000000000, //71
            0b00000000000000000000000000010000, //72 <1001-ADD> GPR -> MAR     [GPR_MAR]  ..........................[EXECUTE]
            0b00000000000000000000000001000000, //73 <1001-ADD> M —> GPR       [M_GPR]  ............................[EXECUTE]
            0b00000000000000000000010000000000, //74 <1001-ADD> GPR+Acc -> Acc [ADD]  ..............................[EXECUTE]
            0b01000000000000000000000000000000, //75 <1001-ADD> 0 -> SC        [CLRSC]  ............................[EXECUTE]
            0b00000000000000000000000000000000, //76
            0b00000000000000000000000000000000, //77
            0b00000000000000000000000000000000, //78
            0b00000000000000000000000000000000, //79
            0b00000000000000000000000000010000, //80 <1010-ADDI> GPR -> MAR     [GPR_MAR]  .........................[EXECUTE]
            0b00000000000000000000000001000000, //81 <1010-ADDI> M —> GPR       [M_GPR]  ...........................[EXECUTE]
            0b00000000000000000000000000010000, //82 <1010-ADDI> GPR -> MAR     [GPR_MAR]  .........................[EXECUTE]
            0b00000000000000000000000001000000, //83 <1010-ADDI> M —> GPR       [M_GPR]  ...........................[EXECUTE]
            0b00000000000000000000010000000000, //84 <1010-ADDI> GPR+Acc -> Acc [ADD]  .............................[EXECUTE]
            0b01000000000000000000000000000000, //85 <1010-ADDI> 0 -> SC        [CLRSC]  ...........................[EXECUTE]
            0b00000000000000000000000000000000, //86
            0b00000000000000000000000000000000, //87
            0b00000000000000000000000000010000, //88 <1011-STA> GPR -> MAR      [GPR_MAR]  .........................[EXECUTE]
            0b00000000000000000000000010000000, //89 <1011-STA> Acc —> GPR      [A_GPR]    .........................[EXECUTE]
            0b00000000000000000000000000000001, //90 <1011-STA> GPR -> M        [GPR_M]    .........................[EXECUTE]
            0b01000000000000000000000000000000, //91 <1011-STA> 0 -> SC         [CLRSC]   ..........................[EXECUTE]
            0b00000000000000000000000000000000, //92
            0b00000000000000000000000000000000, //93
            0b00000000000000000000000000000000, //94
            0b00000000000000000000000000000000, //95
            0b00000000000000000000000000000100, //96 <1100-JMP> GPR -> PC       [GPR_PC]   .........................[EXECUTE]
            0b01000000000000000000000000000000, //97 <1100-JMP> 0 -> SC         [CLRSC]   ..........................[EXECUTE]
            0b00000000000000000000000000000000, //98
            0b00000000000000000000000000000000, //99
            0b00000000000000000000000000000000, //100
            0b00000000000000000000000000000000, //101
            0b00000000000000000000000000000000, //102
            0b00000000000000000000000000000000, //103
            0b00000000000000000000000000010000, //104 <1101-JMPI> GPR -> MAR    [GPR_MAR]  .........................[EXECUTE]
            0b00000000000000000000000001000000, //105 <1101-JMPI> M —> GPR      [M_GPR]  ...........................[EXECUTE]
            0b00000000000000000000000000000100, //106 <1101-JMPI> GPR -> PC     [GPR_PC]   .........................[EXECUTE]
            0b01000000000000000000000000000000, //107 <1101-JMPI> 0 -> SC       [CLRSC]   ..........................[EXECUTE]
            0b00000000000000000000000000000000, //108
            0b00000000000000000000000000000000, //109
            0b00000000000000000000000000000000, //110
            0b00000000000000000000000000000000, //111
            0b00000000000000000000000000010000, //112 <1110-CSR> GPR -> MAR             [GPR_MAR]     ..............[EXECUTE]
            0b00000000000000000000000100000000, //113 <1110-CSR> PC -> GPR              [PC_GPR]   .................[EXECUTE]
            0b00000000000000000000000000000001, //114 <1110-CSR> GPR -> M               [GPR_M]    .................[EXECUTE]
            0b00000000000000000000000000000010, //115 <1110-CSR> PC+1 -> PC             [INCPC]    .................[EXECUTE]
            0b01000000000000000000000000000000, //116 <1110-CSR> 0 -> SC                [CLRSC]    .................[EXECUTE]
            0b00000000000000000000000000000000, //117
            0b00000000000000000000000000000000, //118
            0b00000000000000000000000000000000, //119
            0b00000000000000000000000000010000, //120 <1111-ISZ> GPR -> MAR          [GPR_MAR]  ....................[EXECUTE]
            0b00000000000000000000000001000000, //121 <1111-ISZ> M -> GPR            [M_GPR]    ....................[EXECUTE]
            0b00000000000000000000001000000000, //122 <1111-ISZ> GPR+1 -> GPR        [INCGPR]   ....................[EXECUTE]
            0b00000000000000000000000000000001, //123 <1111-ISZ> GPR -> M            [GPR_M]    ....................[EXECUTE]
            0b00100000000000000000000000000000, //124 <1111-ISZ> PC+1 -> PC          [INCPCZ]   ....................[EXECUTE]
            0b01000000000000000000000000000000, //125 <1111-ISZ> 0 -> SC             [CLRSC]    ....................[EXECUTE]
            0b00000000000000000000000000000000, //126
            0b00000000000000000000000000000000, //127
            0b10000000000000000000000000000000, //128 <10000-HLT> HLT                [HLT]      ....................[EXECUTE]
            0b01000000000000000000000000000000, //129 <10000-HLT> 0 -> SC            [CLRSC]    ....................[EXECUTE]
            0b00000000000000000000000000000000, //130
            0b00000000000000000000000000000000, //131
            0b00000000000000000000000000000000, //132
            0b00000000000000000000000000000000, //133
            0b00000000000000000000000000000000, //134
            0b00000000000000000000000000000000, //135
            };


            BYTE n_size = sizeof(Microprg)/sizeof(DWORD);
            std::cout << "\n****** WORD n_size ****** " << (int)n_size << "\n";

            for (int i = 0; i < n_size; i++) // load Microcode to memory
            {
                mMicrocode[i] =  Microprg[i];
                std::cout << (DWORD)mMicrocode[i] << " - " ;
            }
	}

	void RisingEdge()
	{
        (mRegFlg->Get() & 0x01) ?  mFlagF = 1 : mFlagF = 0;
        (mRegFlg->Get() & 0x02) ?  mFlagZ = 1 : mFlagZ = 0;

		if (mRegSteps->Get() > 2)
        {
            mOpcode = (BYTE)(mRegOpr->Get());
            (mOpcode == 0) ? mHltBit = 0b1000'0000 : mHltBit = 0b0000'0000;

            mControlBus = mMicrocode[((mOpcode << 3) | mHltBit | (mRegSteps->Get()-3))];

        }
        else
        {
            mControlBus = mMicrocode[mRegSteps->Get()];
        }

        if (mControlBus & INCPCF)				// SFZ
		{
			if (!mFlagF)
            {
                mControlBus |= INCPC;
            }

		}

		if (mControlBus & INCPCZ)				// ISZ
		{
			if (mFlagZ)
            {
                mControlBus |= INCPC;
            }

		}

        if (mControlBus & CLRSC)				// immediate asnychroneous reset
		{
			mRegSteps->Reset();
			mControlBus = mMicrocode[0];
		}

		if (mControlBus & HLT)
		{
			exit(0);
		}

		std::cout << "\nSeq Count ->" << mRegSteps->Get() << "\n";
		std::cout << "\nOpCode ->" << (WORD)mOpcode << "\n";
		std::cout << "\nControl Bus = " << mControlBus << "\n";

	}

    void Reset()
    {
        //mControlBus = 0;
    }

};


class Computer
{
protected:
	uint32_t mLastTicks;
	float mSimTime;
    WORD mData;
    WORD mBusLines;
    DWORD mCtrlLines;
    std::vector<std::shared_ptr<Component>> mComponents;

public:
	Computer()
	{

        // 0b[HLT][CLRSC][INCPCZ][INCPCF]0000000000[INCA][COMA][COMF][CLRF][ROL][ROR][CLRA][ADD][INCGPR][PC_GPR][A_GPR][M_GPR][GPR_OP][GPR_MAR][PC_MAR][GPR_PC][INCPC][GPR_M]

        std::shared_ptr<Register> accReg = std::make_shared<Register>(mBusLines, mCtrlLines, 0, A_GPR, INCA, 0x0fff);
        std::shared_ptr<Register> gprReg = std::make_shared<Register>(mBusLines, mCtrlLines, (PC_GPR | A_GPR | M_GPR), (GPR_M | GPR_MAR | GPR_OP | GPR_PC), INCGPR, 0x0fff);
        std::shared_ptr<Register> pcReg = std::make_shared<Register>(mBusLines, mCtrlLines, GPR_PC, (PC_MAR | PC_GPR), INCPC, 0x00ff);
        std::shared_ptr<Register> flgReg = std::make_shared<Register>(mBusLines, mCtrlLines, 0, 0, 0, 0x0003);
        std::shared_ptr<Register> marReg = std::make_shared<Register>(mBusLines, mCtrlLines, (PC_MAR | GPR_MAR), 0, 0, 0x00ff);

        std::shared_ptr<Register> oprReg = std::make_shared<Register>(mBusLines, mCtrlLines, 0, 0, 0, 0x000f);
        std::shared_ptr<Register> sReg = std::make_shared<Register>(mBusLines, mCtrlLines, 0, 0, 0xffff, 0x000f);	// always counting

        std::shared_ptr<GprBus> gBus = std::make_shared<GprBus>(mBusLines, mCtrlLines, (PC_GPR | GPR_OP), 0, pcReg, oprReg, gprReg);
        std::shared_ptr<Adder> alu = std::make_shared<Adder>(mBusLines, mCtrlLines, (ADD | ROL | ROR | COMA | COMF | CLRA | CLRF), 0 , accReg, gprReg, flgReg);
        std::shared_ptr<Memory> ram = std::make_shared<Memory>(mBusLines, mCtrlLines, GPR_M, M_GPR, marReg);
        std::shared_ptr<Control> ctrl = std::make_shared<Control>(mBusLines, mCtrlLines, oprReg, sReg, flgReg);


        mComponents.emplace_back(sReg);
        mComponents.emplace_back(pcReg);
        mComponents.emplace_back(accReg);
        mComponents.emplace_back(gprReg);
        mComponents.emplace_back(flgReg);
        mComponents.emplace_back(oprReg);
        mComponents.emplace_back(marReg);
        mComponents.emplace_back(gBus);
        mComponents.emplace_back(alu);
        mComponents.emplace_back(ram);
        mComponents.emplace_back(ctrl);

		Reset();

	}

	void Reset()
	{
		for (auto& c : mComponents) c->Reset();
		mSimTime = 0.0f;
		mLastTicks = GetTickCount();
	}

	void Update()
	{
		uint32_t nowticks = GetTickCount();
		mSimTime += (nowticks - mLastTicks)*0.001f;
		mLastTicks = nowticks;
		while (mSimTime > 1.0f / 1843200.0f)
		{
			for(auto& c : mComponents) c->RisingEdge();
            for(auto& c : mComponents) c->HighLevel();
            for(auto& c : mComponents) c->FallingEdge();
            for(auto& c : mComponents) c->LowLevel();

			mSimTime -= 1.0f / 1843200.0f;
		}
	}
};


int main()
{

    std::cout << "\n**********************************  CPU   **********************************\n";

    Computer cpu;
	bool running = true;
	cpu.Reset();
	while (running)
	{
        cpu.Update();
		Sleep(1);
	}

    return 0;
}

