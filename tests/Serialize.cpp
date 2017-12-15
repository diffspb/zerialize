//
// Created by diff on 16.11.17.
//

#include "gtest.h"
#include "Serialize.h"
#include <limits>

inline bool operator==(struct timeval t0, struct timeval t1)
{
        return t0.tv_sec == t1.tv_sec && t0.tv_usec == t1.tv_usec;
}

using namespace serialize;

template <typename T>
void check_arithmetic_type()
{
	uint16_t id = 1;

	T values[3] = {0, numeric_limits<T>::min(), numeric_limits<T>::max()};

	SerializeHelper serialize_helper(0);
	for (auto v: values) {
		serialize_helper.valueAndIdToBuf(id++, v);
	}

	T readed_value;
	auto serialized_data = serialize_helper.getSerializedData();
	DeserializeHelper deserialize_helper(serialized_data);
	for (int i = 0; i < 3; i++) {
		ASSERT_TRUE(deserialize_helper.getData(1 + i, readed_value));
		EXPECT_EQ(readed_value, values[i]);
	}
}

TEST(SerializeTest, ArithmeticTypes)
{
	check_arithmetic_type<bool>();
	check_arithmetic_type<uint8_t>();
	check_arithmetic_type<int8_t>();
	check_arithmetic_type<uint16_t>();
	check_arithmetic_type<int16_t>();
	check_arithmetic_type<uint32_t>();
	check_arithmetic_type<int32_t>();
	check_arithmetic_type<float>();
	check_arithmetic_type<double>();
}

TEST(SerializeTest, TimeVal)
{
	uint16_t id = 1;
	const timeval timeval_values[4] =
			{{0, 0}, {12345678, 0}, {0, 12345678}, {43215678, 87654321}};

	SerializeHelper serialize_helper(0);
	for (auto a: timeval_values) {
		serialize_helper.valueAndIdToBuf(id++, a);
	}

	auto serialized_data = serialize_helper.getSerializedData();

	DeserializeHelper deserialize_helper(serialized_data);

	timeval readed_value;

	for (int i = 0; i < 4; i++) {
		EXPECT_TRUE(deserialize_helper.getData(i+1, readed_value));
		EXPECT_EQ(readed_value, timeval_values[i]);
	}
}

TEST(SerializeTest, String)
{
	uint16_t id = 1;
	const string strings[4] =
			{"", "String1", "String2", "Super long string with \0 inside"};

	SerializeHelper serialize_helper(0);
	for (auto str: strings) {
		serialize_helper.valueAndIdToBuf(id++, str);
	}

	auto serialized_data = serialize_helper.getSerializedData();

	DeserializeHelper deserialize_helper(serialized_data);

	string readed_value;
	for (int i = 0; i < 4; i++) {
		EXPECT_TRUE(deserialize_helper.getData(1 + i, readed_value));
		EXPECT_EQ(readed_value, strings[i]);
	}

}

TEST(SerializeTest, MagicNumber)
{
	constexpr int magic_0 = 0;
	constexpr int magic_1 = 0xDEADBEAF;
	constexpr int magic_2 = 12345678;
	constexpr int magic_3 = 0xFFFFFFFF;

	SerializeHelper serialize_helper_0(magic_0);
	SerializeHelper serialize_helper_1(magic_1);
	SerializeHelper serialize_helper_2(magic_2);
	SerializeHelper serialize_helper_3(magic_3);

	auto serialized_data = serialize_helper_0.getSerializedData();
	DeserializeHelper deserialize_helper_0(serialized_data);
	serialized_data = serialize_helper_1.getSerializedData();
	DeserializeHelper deserialize_helper_1(serialized_data);
	serialized_data = serialize_helper_2.getSerializedData();
	DeserializeHelper deserialize_helper_2(serialized_data);
	serialized_data = serialize_helper_3.getSerializedData();
	DeserializeHelper deserialize_helper_3(serialized_data);

	EXPECT_TRUE(deserialize_helper_0.isMagicCorrect(magic_0));
	EXPECT_TRUE(deserialize_helper_1.isMagicCorrect(magic_1));
	EXPECT_TRUE(deserialize_helper_2.isMagicCorrect(magic_2));
	EXPECT_TRUE(deserialize_helper_3.isMagicCorrect(magic_3));
}

TEST(SerializeTest, MultipleTypes)
{
	uint16_t id = 1;
	const int int_value = 789;
	const timeval timeval_value = {123, 456};
	const string string_value = "Test string";

	SerializeHelper serialize_helper(0);
	serialize_helper.valueAndIdToBuf(id++, int_value);
	serialize_helper.valueAndIdToBuf(id++, string_value);
	serialize_helper.valueAndIdToBuf(id++, timeval_value);

	auto serialized_data = serialize_helper.getSerializedData();
	DeserializeHelper deserialize_helper(serialized_data);

	int readed_int = 0;
	timeval readed_timeval = {0, 0};
	string readed_string;

	EXPECT_TRUE(deserialize_helper.getData(1, readed_int));
	EXPECT_TRUE(deserialize_helper.getData(2, readed_string));
	EXPECT_TRUE(deserialize_helper.getData(3, readed_timeval));

	EXPECT_EQ(readed_int, int_value);
	EXPECT_EQ(readed_string, string_value);
	EXPECT_EQ(readed_timeval, timeval_value);
}

const uint32_t default_data_values[4] = {123, 0, 321, 123456789};
const uint32_t default_magic = 12349876;
const string default_string_value = "Test string";

SerializedDataBuffer get_buffer_with_data()
{
	uint16_t id = 1;
	SerializeHelper serialize_helper(default_magic);
	for (auto v: default_data_values) {
		serialize_helper.valueAndIdToBuf(id++, v);
	}
	serialize_helper.valueAndIdToBuf(id++, default_string_value);
	return serialize_helper.getSerializedData();
}

bool check_buffer_with_data(SerializedDataBuffer &buffer)
{
	uint32_t readed_int;
	string readed_string;
	bool res = true;

	DeserializeHelper deserialize_helper(buffer);

	if (!deserialize_helper.isMagicCorrect(default_magic)) {
		return false;
	}

	for (int i = 0; i < 4; i++) {
		res = res && deserialize_helper.getData(1 + i, readed_int);
		res = res && (readed_int == default_data_values[i]);
	}

	res = res && deserialize_helper.getData(5, readed_string);
	res = res && (readed_string == default_string_value);

	return res;
}

template <typename T>
void get_data_from_buff(uint8_t *buffer, unsigned offset, T &t)
{
	memcpy(&t, buffer + offset, sizeof(T));
}

template <typename T>
void put_data_into_buff(uint8_t *buffer, unsigned offset, T &t)
{
	memcpy(buffer + offset, &t, sizeof(T));
}

TEST(SerializeTest, IsDatasetCorrect)
{
	auto serialized_buff = get_buffer_with_data();
	EXPECT_TRUE(check_buffer_with_data(serialized_buff));
}

TEST(SerializeTest, SizeIsSmaller)
{
	auto serialized_buff = get_buffer_with_data();

	uint32_t data32 = 17;
	// размер лежит по смещению 4
	put_data_into_buff(serialized_buff.data(), 4, data32);

	EXPECT_FALSE(check_buffer_with_data(serialized_buff));
}

TEST(SerializeTest, SizeIsBigger)
{
	auto serialized_buff = get_buffer_with_data();

	uint32_t data32 = 95;
	// размер лежит по смещению 4
	put_data_into_buff(serialized_buff.data(), 4, data32);

	EXPECT_FALSE(check_buffer_with_data(serialized_buff));
}

TEST(SerializeTest, WrongElementSize)
{
	auto serialized_buff = get_buffer_with_data();

	uint32_t data32 = 5; // 5 bytes
	// размер элемента лежит по смещению 10
	put_data_into_buff(serialized_buff.data(), 10, data32);

	EXPECT_FALSE(check_buffer_with_data(serialized_buff));
}

TEST(SerializeTest, WrongElementId)
{
	auto serialized_buff = get_buffer_with_data();

	uint16_t data16 = 5; // id 5
	// идентификатор элемента лежит по смещению 8
	put_data_into_buff(serialized_buff.data(), 8, data16);

	EXPECT_FALSE(check_buffer_with_data(serialized_buff));
}

TEST(SerializeTest, PiecesPositions)
{
	auto serialized_buff = get_buffer_with_data();

	uint16_t uint16;
	uint32_t uint32;

	get_data_from_buff(serialized_buff.data(), 0, uint32);
	EXPECT_EQ(uint32, default_magic);
	get_data_from_buff(serialized_buff.data(), 4, uint32);
	// ожидаемый размер буфера :
	EXPECT_EQ(uint32, 8u + 4u * (2 + 4 + 4) + (2 + 4 + 11));
	for (unsigned i = 0; i < 4; i++) {
		unsigned offset = 8 + (2 + 4 + 4) * i;
		get_data_from_buff(serialized_buff.data(), offset, uint16);
		EXPECT_EQ(uint16, i + 1); // проверка идентификатора элемента
		get_data_from_buff(serialized_buff.data(), offset + 2, uint32);
		EXPECT_EQ(uint32, 4u); // проверка размера элемента
		get_data_from_buff(serialized_buff.data(), offset + 6, uint32);
		EXPECT_EQ(uint32,
				default_data_values[i]); // проверка значения элемента
	}
	unsigned str_offs = 8 + 4 * (2 + 4 + 4);
	get_data_from_buff(serialized_buff.data(), str_offs, uint16);
	EXPECT_EQ(uint16, 5u); // проверка идентификатора элемента со строкой
	get_data_from_buff(serialized_buff.data(), str_offs + 2, uint32);
	EXPECT_EQ(uint32, 11u); // проверка размера элемента со строкой
	// проверка значения элемента со строкой
	EXPECT_TRUE(strncmp(default_string_value.c_str(),
			(char*)serialized_buff.data() + str_offs + 2 + 4, 11) == 0);
}


