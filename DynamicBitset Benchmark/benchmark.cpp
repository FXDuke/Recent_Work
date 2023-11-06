#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

const uint64_t mINT64_MAX = 0x7FFFFFFFFFFFFFFF;
const uint64_t i64 = 64;
class DynamicBitset {

private:

    uint64_t Size;
    uint64_t* Storage = new uint64_t[0];

public:

    // @Overload

    bool operator==(DynamicBitset& Target)
    {
        if (Target.getSize() != Size)
            return false;
        for (int Index = 0; Index < Size; Index++)
        {
            if (Target.getValue(Index) != Storage[Index])
                return false;
        }
        return true;
    }

    bool operator!=(DynamicBitset& Target)
    {
        if (Target.getSize() != Size)
            return true;
        for (int Index = 0; Index < Size; Index++)
        {
            if (Target.getValue(Index) != Storage[Index])
                return true;
        }
        return false;
    }

    DynamicBitset operator|(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = Storage[Index] | Target.getValue(Index);
        return Result;
    }

    DynamicBitset operator^(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = Storage[Index] ^ Target.getValue(Index);
        return Result;
    }

    DynamicBitset operator&(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        DynamicBitset Result(TSize * i64);
        for (uint64_t Index = 0; Index < TSize; Index++)
            Result.Storage[Index] = Storage[Index] & Target.getValue(Index);
        return Result;
    }

    DynamicBitset operator~()
    {
        DynamicBitset Result(Size * i64);
        for (uint64_t Index = 0; Index < Size; Index++)
            Result.Storage[Index] = mINT64_MAX ^ Storage[Index];
        return Result;
    }

    DynamicBitset operator<<(int Amount)
    {
        DynamicBitset Result(Size * i64);

        int ShiftAmount = Amount & 63;
        int Dif = 64 - ShiftAmount;
        Amount >>= 6;

        for (int Index = Size - 1; Index > -1; Index--)
            Result.Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

        for (int Index = Size - 1; Index > -1; Index--)
            Result.Storage[Index] = (Result.Storage[Index] << ShiftAmount) | ((Index - 1 > -1) ? Result.Storage[Index - 1] >> Dif : 0);

        return Result;
    }

    DynamicBitset operator>>(int Amount)
    {
        DynamicBitset Result(Size * i64);

        int ShiftAmount = Amount & 63;
        int Dif = 64 - ShiftAmount;
        Amount >>= 6;

        for (int Index = 0; Index < Size; Index++)
            Result.Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

        for (int Index = 0; Index < Size; Index++)
            Result.Storage[Index] = (Result.Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Result.Storage[Index + 1] << Dif : 0);

        return Result;
    }

    void operator<<=(int Amount)
    {

        int ShiftAmount = Amount & 63;
        int Dif = 64 - ShiftAmount;
        Amount >>= 6;

        for (int Index = Size - 1; Index > -1; Index--)
            Storage[Index] = (Index - Amount > -1) ? Storage[Index - Amount] : 0;

        for (int Index = Size - 1; Index > -1; Index--)
            Storage[Index] = (Storage[Index] << ShiftAmount) | ((Index - 1 > -1) ? Storage[Index - 1] >> Dif : 0);

    }

    void operator>>=(int Amount)
    {

        int ShiftAmount = Amount & 63;
        int Dif = 64 - ShiftAmount;
        Amount >>= 6;

        for (int Index = 0; Index < Size; Index++)
            Storage[Index] = (Index + Amount < Size) ? Storage[Index + Amount] : 0;

        for (int Index = 0; Index < Size; Index++)
            Storage[Index] = (Storage[Index] >> ShiftAmount) | ((Index + 1 < Size) ? Storage[Index + 1] << Dif : 0);
    }

    void operator|=(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        for (uint64_t Index = 0; Index < TSize; Index++)
            Storage[Index] = Storage[Index] | Target.getValue(Index);
    }

    void operator^=(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        for (uint64_t Index = 0; Index < TSize; Index++)
            Storage[Index] = Storage[Index] ^ Target.getValue(Index);
    }

    void operator&=(DynamicBitset& Target)
    {
        uint64_t TSize = Target.getSize();
        for (uint64_t Index = 0; Index < TSize; Index++)
            Storage[Index] = Storage[Index] & Target.getValue(Index);
    }

    // @Read / Write
    void flip(uint64_t Position)
    {
        Storage[Position >> 6] ^= 1ULL << (Position & 63);
    }

    void write(uint64_t Position, bool Value)
    {
        if (Value)
            Storage[Position >> 6] |= 1ULL << (Position & 63);
        if (!Value)
            Storage[Position >> 6] &= ~(1ULL << (Position & 63));
    }

    bool read(uint64_t Position)
    {
        return Storage[Position >> 6] & (1ULL << (Position & 63));
    }

    void setValue(int Position, uint64_t Value)
    {
        Storage[Position] = Value;
    }

    // @Generics
    void resize(uint64_t MaskSize = i64)
    {
        delete Storage;

        uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
        Size = FinalSize;
        Storage = new uint64_t[FinalSize];
    }

    void clear()
    {
        delete Storage;
        Storage = new uint64_t[Size];
    }

    void free()
    {
        delete Storage;
        Storage = new uint64_t[1];
    }

    bool zero()
    {
        for (uint64_t Index = 0; Index < Size; Index++)
            if (Storage[Index] != 0)
                return false;
        return true;
    }

    string concat()
    {
        stringstream Result;
        Result << "0x";
        for (uint64_t Index = Size - 1; Index > 0; Index--)
            Result << hex << Storage[Index] << "_";
        Result << hex << Storage[0] << "_";
        return Result.str();
    }

    uint64_t getValue(uint64_t Position)
    {
        return Storage[Position];
    }

    uint64_t getSize()
    {
        return Size;
    }

    DynamicBitset clone()
    {
        DynamicBitset Result(Size * i64);
        for (uint64_t Index = 0; Index < Size; Index++)
            Result.Storage[Index] = Storage[Index];
        return Result;
    }

    // @Constructor
    DynamicBitset(uint64_t MaskSize = i64)
    {
        delete Storage;
        uint64_t FinalSize = (MaskSize >> 6) + ((MaskSize & 63) > 0);
        Size = FinalSize;
        Storage = new uint64_t[FinalSize];
    }

};

#include "cmath"
#include "bitset"

int main() {

    const uint64_t AmountMasks = 10000;
    const uint64_t MaskSize = 1000000;
    const uint64_t ShiftRightAmount = 10000;

    cout << "Task:\tMake " << AmountMasks << " bitsets of size " << MaskSize << " and set each value to 1 manually, then shift right "<< ShiftRightAmount <<", then free data " << endl;

    clock_t DynamicBitsetTime = clock();

    vector<DynamicBitset> MyMasks(AmountMasks);

    for (int Index = 0; Index < AmountMasks; Index++)
    {
        DynamicBitset NewMask = MyMasks[Index];
        NewMask.resize(MaskSize);


        for (int X = 0; X < MaskSize; X++)
            NewMask.flip(X);

        NewMask >>= ShiftRightAmount;

        MyMasks[Index] = NewMask;
    }

    for (DynamicBitset x : MyMasks)
        x.free();

    double DynamicBitsetTimeFinal = ((double)clock() - DynamicBitsetTime) / CLOCKS_PER_SEC;
    cout << "Dynamic Bitset Elapsed: " << DynamicBitsetTimeFinal << endl;


    clock_t BitsetTime = clock();
    vector<bitset<MaskSize>> Masks(AmountMasks);

    for (int Index = 0; Index < AmountMasks; Index++)
    {
        bitset<MaskSize> NewMask;

        for (int X = 0; X < MaskSize; X++)
            NewMask[X] = 1;

        NewMask >>= ShiftRightAmount;

        Masks[Index] = NewMask;
    }

    double BitsetTimeFinal = ((double)clock() - BitsetTime) / CLOCKS_PER_SEC;
    cout << "Bitset Elapsed: " << BitsetTimeFinal << endl;

    cout << "Winner: " << ((DynamicBitsetTimeFinal < BitsetTimeFinal) ? "Dynamic Bitset" : "Normal Bitset") << endl;


    return 0;
}
