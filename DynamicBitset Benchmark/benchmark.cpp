#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

const uint64_t mINT64_MAX = 0x7FFFFFFFFFFFFFFF;
const uint64_t i64 = 64;
class DynamicBitset {

private:

    uint64_t Size;
    vector<uint64_t> Storage;

public:

    // @Operations
    DynamicBitset Or(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = this->Storage[Index] | Target.getValue(Index);
        return Result;
    }

    DynamicBitset Xor(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = this->Storage[Index] ^ Target.getValue(Index);
        return Result;
    }

    DynamicBitset And(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = this->Storage[Index] & Target.getValue(Index);
        return Result;
    }

    DynamicBitset Not()
    {
        DynamicBitset Result(this->Size * i64);
        for (uint64_t Index = 0; Index < this->Size; Index++)
            Result.Storage[Index] = mINT64_MAX ^ this->Storage[Index];
        return Result;
    }

    void shiftLeft(int Amount)
    {

        if (Amount < 0)
            return this->shiftRight(-Amount);

        int ShiftAmount = Amount & 63;
        int Dif = 64 - ShiftAmount;
        Amount >>= 6;

        for (int Index = this->Size - 1; Index > -1; Index--)
            this->Storage[Index] = (Index - Amount > -1) ? this->Storage[Index - Amount] : 0;

        if (ShiftAmount > 0)
        {
            for (int Index = this->Size - 1; Index > -1; Index--)
            {
                this->Storage[Index] <<= ShiftAmount;
                this->Storage[Index] |= (Index - 1 > -1) ? this->Storage[Index - 1] >> Dif : 0;
            }
        }

    }

    void shiftRight(int Amount)
    {

        if (Amount < 0)
            return this->shiftLeft(-Amount);

        int Remainder = Amount & 63;
        int Dif = 64 - Remainder;
        Amount >>= 6;

        for (int Index = 0; Index < this->Size; Index++)
            this->Storage[Index] = (Index+Amount < this->Size) ? this->Storage[Index + Amount] : 0;
        
        if (Remainder > 0)
        {
            for (int Index = 0; Index < this->Size; Index++)
            {
                this->Storage[Index] >>= Remainder;
                this->Storage[Index] |= (Index + 1 < this->Size) ? this->Storage[Index + 1] << Dif : 0;
            }
        }
    }

    // @Read / Write
    void flip(uint64_t Position)
    {
        this->Storage[Position >> 6] ^= 1ULL << (Position & 63);
    }

    void write(uint64_t Position, bool Value)
    {
        if (Value)
            this->Storage[Position >> 6] |= 1ULL << (Position & 63);
        if (!Value)
            this->Storage[Position >> 6] &= ~(1ULL << (Position & 63));
    }

    bool read(uint64_t Position)
    {
        return this->Storage[Position >> 6] & (1ULL << (Position & 63));
    }

    void setValue(int Position, uint64_t Value)
    {
        this->Storage[Position] = Value;
    }

    // @Generics
    void resize(uint64_t MaskSize = i64)
    {
        uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
        this->Size = FinalSize;
        this->Storage = vector<uint64_t>(FinalSize, 0);
    }

    void clear()
    {
        this->Storage = vector<uint64_t>(this->Size, 0);
    }

    bool zero()
    {
        for (uint64_t Index = 0; Index < this->Size; Index++)
            if (this->Storage[Index] > 0)
                return true;
        return false;
    }

    string concat()
    {
        stringstream Result;
        Result << "0x";
        for (uint64_t Index = this->Size - 1; Index > 0; Index--)
            Result << hex << this->Storage[Index] << "_";
        Result << hex << this->Storage[0] << "_";
        return Result.str();
    }

    uint64_t getValue(uint64_t Position)
    {
        return this->Storage[Position];
    }

    uint64_t getSize()
    {
        return this->Size;
    }

    DynamicBitset clone()
    {
        DynamicBitset Result(this->Size * i64);
        for (uint64_t Index = 0; Index < this->Size; Index++)
            Result.Storage[Index] = this->Storage[Index];
        return Result;
    }

    // @Constructor
    DynamicBitset(uint64_t MaskSize = i64)
    {
        uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
        this->Size = FinalSize;
        this->Storage = vector<uint64_t>(FinalSize, 0);
    }

};

#include <ctime>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "cmath"

int main() {

    uint64_t NumberMasks2 = 10000;
    uint64_t MaskSize2 = 10000;

    cout << "Create x Bitsets with size of y, and set every value to rand()&0x1,\nNumber of Bitsets:\t " << NumberMasks2 << "\nSize of each Bitset: " << MaskSize2 << "\n\n";

    clock_t Begin = clock();
    vector<DynamicBitset> Masks(NumberMasks2);

    static uint64_t Mask = 0;
    for (uint64_t Index = 0; Index < NumberMasks2; Index++)
    {
        DynamicBitset NewMask(MaskSize2);

        for (uint64_t x = 0; x < MaskSize2; x++)
            NewMask.write(x, rand() & 0x1);

        Masks[Index] = NewMask;
    }

    double MyTime = ((double)clock() - Begin) / CLOCKS_PER_SEC;
    cout << "Dynamic Bitset Elapsed Time: " << MyTime << "\n";

    Begin = clock();
    vector<vector<bool>> Other(NumberMasks2);

    for (uint64_t Index = 0; Index < NumberMasks2; Index++)
    {
        vector<bool> NewMask(MaskSize2, 0);

        uint64_t x;
        for (x = 0; x < MaskSize2; x++)
            NewMask[x] = rand() & 0x1;

        Other[Index] = NewMask;
    }

    double VectorTime = ((double)clock() - Begin) / CLOCKS_PER_SEC;
    cout << "Vector Bitset Elapsed Time:  " << VectorTime << "\n";

    cout << "\nWinner: " << ((VectorTime > MyTime) ? "Dynamic Bitset" : "Vector Bitset");


    return 0;
}
