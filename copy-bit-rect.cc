//#include "pch.h"

#include <stdio.h>
#include <cassert>
#include <cstdint>
#include <vector>
#include <algorithm>



using namespace std;

struct Size {
  int w;
  int h;
};

struct Rect {
  int x;
  int y;
  int w;
  int h;
};

// You are provided with a bitmap buffer `src`, from which you need to copy out
// a _not necessarily byte aligned_ set of bits defined by `area` into `out`.
//
// You can assume the following:
// 1. Source size is expressed in bytes. That is, a 40x4 bitmap has size of 5x4
//    8-bit bytes.
// 2. X coordinate of the area rect is expressed in bits. That is, offset 2
//    means 3rd most significant (leftmost) bit of the first byte.
// 3. Width of the area (`w`) is always divisible by 8.
// 4. Rect of the area always fits within the buffer, you don't have to handle
//    clipping.
//
// A number of test cases is provided. You need to fill in the code in the
// function below. Your code is expected to compile and pass tests. Give it
// your best shot.

void CopyBitRect(const uint8_t* src, Size size, Rect area, uint8_t* out)
{
    // data to copy
    int bitstocopy = (area.w * area.h);

    // start loc in src matrix
    int startbitpos = (area.y * size.w * 8) + area.x;

    // manipulatable pointer to src 
    uint8_t *psrc = (uint8_t *)(src + (startbitpos / 8));

    // byte boundary overflow
    uint8_t overflow = (startbitpos % 8);

    // copy byte by byte, yuk, until full RECT copied
    while (bitstocopy)
    {
        // copy current byte and update source pointer
        *out = *psrc++;

        // adjust data for non byte-boundary start position
        // want to copy rightmost (least sig) 8-overflow bits of first uint as high order bits
        // leftmost (most sig) overflow bits from next uint fill low order bits
        if (overflow != 0)
        {
            *out = ((*out << overflow) | (*psrc >> (8 - overflow)));
        }

        // update progress
        out++;
        bitstocopy -= 8;

        // switch to new row when necesssary
        if ((bitstocopy % area.w) == 0)
        {
            psrc += (((size.w * 8) - area.w) / 8);
        }
    }

    // as i read it, size won't overflow, so done
}

void AssertVectorsEqual(const vector<uint8_t>& left,
                        const vector<uint8_t>& right) {
  assert(left.size() == right.size());
  for (int i = 0; i < left.size(); ++i) {
    assert(left[i] == right[i]);
  }
}

void RunTests() {
  const Size kInputSize{5, 4};
  // Bitmap representation:
  // 00 10 00 00 10000000 10110011 1001001011101010
  // 1100001000111101101000011101000111001011
  // 1010011110010000010111100110110101001001
  // 0011001100000100010110011111000010000000
  const vector<uint8_t> kInput{
      32,  128, 179, 146, 234,  //
      194, 61,  161, 209, 203,  //
      167, 144, 94,  109, 73,   //
      51,  4,   89,  240, 128,  //
  };

  // Area is not byte-aligned.
  {
    const Rect kArea{5, 0, 16, 3};
    // [0001000000010110]
    // [0100011110110100]
    // [1111001000001011]
    vector<uint8_t> kReferenceResult{
        16,  22,   //
        71,  180,  //
        242, 11,   //
    };
    vector<uint8_t> result(kReferenceResult.size());
    CopyBitRect(kInput.data(), kInputSize, kArea, result.data());
    AssertVectorsEqual(result, kReferenceResult);
  }
  // Leftmost column, byte aligned
  {
    const Rect kArea{0, 0, 8, 4};
    // [00100000]
    // [11000010]
    // [10100111]
    // [00110011]
    vector<uint8_t> kReferenceResult{
        32,   //
        194,  //
        167,  //
        51,   //
    };
    vector<uint8_t> result(kReferenceResult.size());
    CopyBitRect(kInput.data(), kInputSize, kArea, result.data());
    AssertVectorsEqual(result, kReferenceResult);
  }
  // Rightmost column, byte aligned.
  {
    const Rect kArea{4 * 8, 0, 8, 4};
    // [11101010]
    // [11001011]
    // [01001001]
    // [10000000]
    vector<uint8_t> kReferenceResult{
        234,  //
        203,  //
        73,   //
        128,  //
    };
    vector<uint8_t> result(kReferenceResult.size());
    CopyBitRect(kInput.data(), kInputSize, kArea, result.data());
    AssertVectorsEqual(result, kReferenceResult);
  }
  // Non-byte aligned area in the lower right corner.
  {
    const Rect kArea{3 * 8 - 1, 2, 16, 2};
    // [0011011010100100]
    // [1111100001000000]
    vector<uint8_t> kReferenceResult{
        54, 164,  //
        248, 64,  //
    };
    vector<uint8_t> result(kReferenceResult.size());
    CopyBitRect(kInput.data(), kInputSize, kArea, result.data());
    AssertVectorsEqual(result, kReferenceResult);
  }
}

int main() { RunTests(); }
