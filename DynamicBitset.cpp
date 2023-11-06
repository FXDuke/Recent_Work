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

    void shiftLeft(uint64_t Amount)
    {

        uint64_t ShiftAmount = Amount & 63;
        uint64_t Dif = 64 - ShiftAmount;

        for (int Index = 0; Index < Amount / 64; Index++)
        {
            for (uint64_t Index = this->Size - 1; Index > 0; Index--)
            {
                this->Storage[Index] = this->Storage[Index - 1];
                this->Storage[Index - 1] = 0;
            }
        }

        if (ShiftAmount == 0)
            return;

        for (uint64_t Index = 0, Remainder = 0, OldRemainder = 0; Index < this->Size; Index++)
        {
            OldRemainder = Remainder;
            Remainder = this->Storage[Index] >> Dif;
            this->Storage[Index] <<= ShiftAmount;
            this->Storage[Index] |= OldRemainder;
        }

    }

    void shiftRight(uint64_t Amount)
    {

        uint64_t Remainder = Amount & 63;
        uint64_t Dif = 64 - Remainder;

        for (int Index = 0; Index < Amount / 64; Index++)
            for (uint64_t Index = 0; Index < this->Size; Index++)
                this->Storage[Index] = this->Storage[Index + 1];
        
        if (Remainder == 0)
            return;

        this->Storage[0] >>= Remainder;
        for (uint64_t Index = this->Size - 1; Index > 1; Index--)
        {
            this->Storage[Index] >>= Remainder;
            this->Storage[Index - 1] |= this->Storage[Index] << Dif;
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
