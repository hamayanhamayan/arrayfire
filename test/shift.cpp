#include <gtest/gtest.h>
#include <arrayfire.h>
#include <af/dim4.hpp>
#include <af/defines.h>
#include <af/traits.hpp>
#include <vector>
#include <iostream>
#include <complex>
#include <string>
#include <testHelpers.hpp>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using af::af_cfloat;
using af::af_cdouble;

template<typename T>
class Shift : public ::testing::Test
{
    public:
        virtual void SetUp() {
            subMat0.push_back({0, 4, 1});
            subMat0.push_back({2, 6, 1});
            subMat0.push_back({0, 2, 1});
        }
        vector<af_seq> subMat0;
};

// create a list of types to be tested
typedef ::testing::Types<float, double, af_cfloat, af_cdouble, int, unsigned int, char, unsigned char> TestTypes;

// register the type list
TYPED_TEST_CASE(Shift, TestTypes);

template<typename T>
void shiftTest(string pTestFile, const unsigned resultIdx,
                 const uint x, const uint y, const uint z, const uint w,
                 bool isSubRef = false, const vector<af_seq> * seqv = nullptr)
{
    vector<af::dim4> numDims;
    vector<vector<T>> in;
    vector<vector<T>> tests;
    readTests<T, T, int>(pTestFile,numDims,in,tests);

    af::dim4 idims = numDims[0];

    af_array inArray = 0;
    af_array outArray = 0;
    af_array tempArray = 0;

    if (isSubRef) {
        ASSERT_EQ(AF_SUCCESS, af_create_array(&tempArray, &(in[0].front()), idims.ndims(), idims.get(), (af_dtype) af::dtype_traits<T>::af_type));

        ASSERT_EQ(AF_SUCCESS, af_index(&inArray, tempArray, seqv->size(), &seqv->front()));
    } else {
        ASSERT_EQ(AF_SUCCESS, af_create_array(&inArray, &(in[0].front()), idims.ndims(), idims.get(), (af_dtype) af::dtype_traits<T>::af_type));
    }

    ASSERT_EQ(AF_SUCCESS, af_shift(&outArray, inArray, x, y, z, w));

    // Get result
    T* outData = new T[tests[resultIdx].size()];
    ASSERT_EQ(AF_SUCCESS, af_get_data_ptr((void*)outData, outArray));

    // Compare result
    size_t nElems = tests[resultIdx].size();
    for (size_t elIter = 0; elIter < nElems; ++elIter) {
        ASSERT_EQ(tests[resultIdx][elIter], outData[elIter]) << "at: " << elIter << std::endl;
    }

    // Delete
    delete[] outData;

    if(inArray   != 0) af_destroy_array(inArray);
    if(outArray  != 0) af_destroy_array(outArray);
    if(tempArray != 0) af_destroy_array(tempArray);
}

#define SHIFT_INIT(desc, file, resultIdx, x, y, z, w)                                       \
    TYPED_TEST(Shift, desc)                                                                 \
    {                                                                                       \
        shiftTest<TypeParam>(string(TEST_DIR"/shift/"#file".test"), resultIdx, x, y, z, w); \
    }

    SHIFT_INIT(Shift0,  shift4d, 0,    2,  0,  0,  0);
    SHIFT_INIT(Shift1,  shift4d, 1,   -1,  0,  0,  0);
    SHIFT_INIT(Shift2,  shift4d, 2,    3,  2,  0,  0);
    SHIFT_INIT(Shift3,  shift4d, 3,   11, 22,  0,  0);
    SHIFT_INIT(Shift4,  shift4d, 4,    0,  1,  0,  0);
    SHIFT_INIT(Shift5,  shift4d, 5,    0, -6,  0,  0);
    SHIFT_INIT(Shift6,  shift4d, 6,    0,  3,  1,  0);
    SHIFT_INIT(Shift7,  shift4d, 7,    0,  0,  2,  0);
    SHIFT_INIT(Shift8,  shift4d, 8,    0,  0, -2,  0);
    SHIFT_INIT(Shift9,  shift4d, 9,    0,  0,  0,  1);
    SHIFT_INIT(Shift10, shift4d, 10,   0,  0,  0, -1);
    SHIFT_INIT(Shift11, shift4d, 11,   1,  1,  1,  1);
    SHIFT_INIT(Shift12, shift4d, 12,  -1, -1, -1, -1);
    SHIFT_INIT(Shift13, shift4d, 13,  21, 21, 21, 21);
    SHIFT_INIT(Shift14, shift4d, 14, -21,-21,-21,-21);