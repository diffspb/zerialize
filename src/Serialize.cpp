//
// Created by diff on 28.09.17.
//

#include "Serialize.h"

using namespace serialize;

struct timeval_32
{
	uint32_t sec;
	uint32_t usec;
};

// ============================================================================
// ====== SerializeHelper
// ============================================================================

SerializeHelper::SerializeHelper(uint32_t magic)
{
	buffer_.resize(256);

	uint32_t data_len = 0;
	addToBuf(magic);
	addToBuf(data_len);
}

SerializedDataBuffer SerializeHelper::getSerializedData()
{
	auto uint32_ptr = reinterpret_cast<uint32_t *>(buffer_.data());
	uint32_ptr[1] = static_cast<uint32_t>(buffer_offset_);
	buffer_.resize(buffer_offset_);

	return std::move(buffer_);
}

void
SerializeHelper::valueAndIdToBuf(uint16_t field_id, const timeval value)
{
	timeval_32 value_u32{ static_cast<uint32_t>(value.tv_sec),
			static_cast<uint32_t>(value.tv_usec) };

	auto value_len = static_cast<uint32_t>(sizeof(value_u32));

	addToBuf(field_id);
	addToBuf(value_len);
	addToBuf(value_u32);
}

void SerializeHelper::valueAndIdToBuf(uint16_t field_id, const string &v)
{
	auto str_len = static_cast<uint32_t>(v.length());

	addToBuf(field_id);
	addToBuf(str_len);
	addToBuf(*v.data(), str_len);
}

void SerializeHelper::idToBuf(uint16_t field_id)
{
	addToBuf(field_id);
}

// ============================================================================
// ========== DeserializeHelper
// ============================================================================

DeserializeHelper::DeserializeHelper(const SerializedDataBuffer &buffer)
		: buffer_(buffer)
{
	if (buffer_.size() >= (sizeof(uint32_t)*2)) {
		// в начале блока лежат идентификатор и длина данных типа uint32_t
		dataFromBuf(magic_);
		dataFromBuf(data_size_);
		// записан неправильный размер
		if (data_size_ > buffer_.size()) {
			data_size_ = 0;
		}
	}

	if (data_size_ != 0) {
		auto old_buffer_offset = buffer_offset_;
		uint16_t id;
		while((id = getNextId()) != 0) {
			id_to_data_.insert(pair<uint16_t, uint32_t>(id, buffer_offset_));
			uint32_t element_size;
			dataFromBuf(element_size);
			buffer_offset_ += element_size;
		}
		buffer_offset_ = old_buffer_offset;
	}
}

bool DeserializeHelper::isMagicCorrect(uint32_t magic)
{
	return (magic_ == magic);
}

uint16_t DeserializeHelper::getNextId()
{
	uint16_t id = 0;
	if (!isDataAvailable(sizeof(id))) {
		return 0;
	}
	dataFromBuf(id);
	return id;
}

bool DeserializeHelper::getData(string &str)
{
	uint32_t len = 0;
	if (!getLen(len)) {
		return false;
	}

	if (!isDataAvailable(len)) {
		return false;
	}

	str = string(
			reinterpret_cast<char*>(buffer_.data() + buffer_offset_), len);
	buffer_offset_ += len;
	return true;
}

bool DeserializeHelper::getData(timeval &tv)
{
	uint32_t len = 0;
	if (!getLen(len)) {
		return false;
	}

	if (!isDataAvailable(len)) {
		return false;
	}

	if (len == sizeof(timeval_32)) {
		timeval_32 tmp = {0, 0};
		dataFromBuf(tmp);
		tv.tv_sec = static_cast<decltype(tv.tv_sec)>(tmp.sec);
		tv.tv_usec = static_cast<decltype(tv.tv_usec)>(tmp.usec);
	} else if (len == sizeof(struct timeval)) {
		dataFromBuf(tv);
	} else {
		return false;
	}

	return true;
}

bool DeserializeHelper::isDataAvailable(unsigned int size)
{
	return ((buffer_offset_ + size) <= (data_size_));
}

bool DeserializeHelper::getLen(uint32_t &len)
{
	if (!isDataAvailable(sizeof(len))) {
		return false;
	}
	dataFromBuf(len);
	return true;
}
