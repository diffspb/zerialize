//
// Created by diff on 28.09.17.
//

#ifndef PROJECT_SERIALIZE_H
#define PROJECT_SERIALIZE_H

#include <cstring>
#include <vector>
#include <map>
#include <memory>

using namespace std;

namespace serialize
{

using SerializedDataBuffer = vector<uint8_t>;

class SerializeHelper
{
public:
	SerializeHelper(uint32_t magic);

	~SerializeHelper() = default;

	// Возвращает буфер с сериализованными данными, после вызова этой функции
	// ничего добавлять уже нельзя
	SerializedDataBuffer getSerializedData();

	// Кладет в буфер простой тип данных вместе с размером и идентификатором
	// и инкрементирует идентификатор, идентификатор не должен быть равен 0
	template<typename T>
	void valueAndIdToBuf(uint16_t field_id, const T &v)
	{
		auto v_size = static_cast<uint32_t>(sizeof(T));
		addToBuf(field_id);
		addToBuf(v_size);
		addToBuf(v);
	}

	// Кладет в буфер структуру timeval вместе с размером и идентификатором
	// и инкрементирует идентификатор, идентификатор не должен быть равен 0
	void valueAndIdToBuf(uint16_t field_id, const timeval value);

	// Кладет в буфер строку вместе с размером и идентификатором
	// и инкрементирует идентификатор, идентификатор не должен быть равен 0
	void valueAndIdToBuf(uint16_t field_id, const string &v);

	// Кладет только идентификатор и инкрементирует его, идентификатор не
	// должен быть равен 0
	void idToBuf(uint16_t field_id);

private:
	template<typename T>
	void addToBuf(T &v, size_t v_size = sizeof(T))
	{
		if (buffer_.size() < (buffer_offset_ + v_size)) {
			buffer_.resize(buffer_.size() * 2);
		}

		memcpy(buffer_.data() + buffer_offset_, &v, v_size);
		buffer_offset_ += v_size;
	}

	SerializedDataBuffer buffer_;
	size_t buffer_offset_ = 0;
};

class DeserializeHelper
{
public:
	explicit DeserializeHelper(const SerializedDataBuffer &buffer);

	bool isMagicCorrect(uint32_t magic);

	bool isDataAvailable(unsigned int size = 1);

	// Читает из буфера идентификатор следующего куска данных
	uint16_t getNextId();

	// Читает из буфера строку и проверяет размер
	bool getData(string &str);

	// Читает из буфера простой тип и проверяет размер
	template<typename T>
	bool getData(T &v)
	{
		uint32_t len = 0;
		if (!getLen(len)) {
			return false;
		}

		if (len != sizeof(T)) {
			return false;
		}

		dataFromBuf(v);

		return true;
	}

	// Читает из буфера структуру timeval
	bool getData(timeval &tv);

	template <typename T>
	bool getData(uint16_t id, T &t)
	{
		auto it = id_to_data_.find(id);
		if (it == id_to_data_.end()) {
			return false;
		}
		auto old_value = buffer_offset_;
		buffer_offset_ = it->second;
		bool res = getData(t);
		buffer_offset_ = old_value;
		return res;
	}

private:
	bool getLen(uint32_t &len);

	template<typename T>
	void dataFromBuf(T &v)
	{
		memcpy(&v, buffer_.data() + buffer_offset_, sizeof(T));
		buffer_offset_ += sizeof(T);
	}

	SerializedDataBuffer buffer_;
	unsigned int buffer_offset_ = 0;
	uint32_t data_size_ = 0;
	uint32_t magic_ = 0;

	map<uint16_t, uint32_t> id_to_data_;
};

}; // namespace serialize

#endif //PROJECT_SERIALIZE_H
