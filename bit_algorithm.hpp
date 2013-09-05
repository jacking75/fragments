/* Copyright (c) 2013, Hillco Psychi, All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** @file
    @author Hillco Psychi (https://twitter.com/psychi)
 */
#ifndef PSYQ_BIT_ALGORITHM_HPP_
#define PSYQ_BIT_ALGORITHM_HPP_
#include <cstdint>

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
namespace psyq
{
	/** @brief �w�肳�ꂽ�ʒu�̃r�b�g�l���擾����B
	    @param[in] in_bits �r�b�g�W���Ƃ��Ĉ��������l�B
	    @param[in] in_position �擾����r�b�g�̈ʒu�B
	    @return �w�肳�ꂽ�ʒu�̃r�b�g�l�B
	 */
	template<typename template_bits>
	bool get_bit(
		template_bits const in_bits,
		std::size_t   const in_position)
	{
		return (in_bits >> in_position) & template_bits(1);
	}

	/** @brief �w�肳�ꂽ�ʒu�Ƀr�b�g�l�Ƃ���1��ݒ肷��B
	    @param[in] in_bits     �r�b�g�W���Ƃ��Ĉ��������l�B
	    @param[in] in_position �ݒ肷��r�b�g�̈ʒu�B
	    @return �w�肳�ꂽ�ʒu�Ƀr�b�g�l��ݒ肵���r�b�g�W���B
	 */
	template<typename template_bits>
	template_bits set_bit(
		template_bits const in_bits,
		std::size_t   const in_position)
	{
		return (template_bits(1) << in_position) | in_bits;
	}

	/** @brief �w�肳�ꂽ�ʒu�Ƀr�b�g�l��ݒ肷��B
	    @param[in] in_bits     �r�b�g�W���Ƃ��Ĉ��������l�B
	    @param[in] in_position �ݒ肷��r�b�g�̈ʒu�B
	    @param[in] in_value    �ݒ肷��r�b�g�l�B
	    @return �w�肳�ꂽ�ʒu�Ƀr�b�g�l��ݒ肵���r�b�g�W���B
	 */
	template<typename template_bits>
	template_bits set_bit(
		template_bits const in_bits,
		std::size_t   const in_position,
		bool          const in_value)
	{
		return psyq::reset_bit(in_bits, in_position)
			| (in_value << in_position);
	}

	/** @brief �w�肳�ꂽ�ʒu�Ƀr�b�g�l�Ƃ���0��ݒ肷��B
	    @param[in] in_bits     �r�b�g�W���Ƃ��Ĉ��������l�B
	    @param[in] in_position �ݒ肷��r�b�g�̈ʒu�B
	    @return �w�肳�ꂽ�ʒu�Ƀr�b�g�l��ݒ肵���r�b�g�W���B
	 */
	template<typename template_bits>
	template_bits reset_bit(
		template_bits const in_bits,
		std::size_t   const in_position)
	{
		return ~(template_bits(1) << in_position) & in_bits;
	}

	/** @brief �w�肳�ꂽ�ʒu�̃r�b�g�l�𔽓]����B
	    @param[in] in_bits     �r�b�g�W���Ƃ��Ĉ��������l�B
	    @param[in] in_position ���]����r�b�g�̈ʒu�B
	    @return �w�肳�ꂽ�ʒu�̃r�b�g�l�𔽓]�����r�b�g�W���B
	 */
	template<typename template_bits>
	template_bits flip_bit(
		template_bits const in_bits,
		std::size_t   const in_position)
	{
		return (template_bits(1) << in_position) ^ in_bits;
	}

	/** @brief ���������_�������r�b�g�W���ɕϊ�����B
	    @param[in] in_float �ϊ����镂�������_�����B
	    @return ���������_�����̃r�b�g�W���B
	 */
	inline uint32_t get_float_bits(
		float const in_float)
	{
		return *reinterpret_cast<std::uint32_t const*>(&in_float);
	}

	/// @copydoc get_float_bits()
	inline std::uint64_t get_float_bits(
		double const in_float)
	{
		return *reinterpret_cast<std::uint64_t const*>(&in_float);
	}

	/** @brief �l��0�ȊO�̃r�b�g�l�̐��𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �l��0�ȊO�̃r�b�g�l�̐��B
	 */
	template<typename template_bits>
	std::size_t count_bit1(
		template_bits const in_bits)
	{
		std::uint32_t local_bits(in_bits);
		local_bits = (local_bits & 0x55555555)
			+ ((local_bits >> 1) & 0x55555555);
		local_bits = (local_bits & 0x33333333)
			+ ((local_bits >> 2) & 0x33333333);
		local_bits = (local_bits & 0x0f0f0f0f)
			+ ((local_bits >> 4) & 0x0f0f0f0f);
		local_bits = (local_bits & 0x00ff00ff)
			+ ((local_bits >> 8) & 0x00ff00ff);
		local_bits = (local_bits & 0x0000ffff)
			+ ((local_bits >>16) & 0x0000ffff);
		return local_bits;
	}

	/// @copydoc count_bit1()
	template<>
	std::size_t count_bit1(
		std::uint64_t const in_bits)
	{
		auto local_bits(in_bits);
		local_bits = (local_bits & 0x5555555555555555LL)
			+ ((local_bits >> 1) & 0x5555555555555555LL);
		local_bits = (local_bits & 0x3333333333333333LL)
			+ ((local_bits >> 2) & 0x3333333333333333LL);
		local_bits = (local_bits & 0x0f0f0f0f0f0f0f0fLL)
			+ ((local_bits >> 4) & 0x0f0f0f0f0f0f0f0fLL);
		local_bits = (local_bits & 0x00ff00ff00ff00ffLL)
			+ ((local_bits >> 8) & 0x00ff00ff00ff00ffLL);
		local_bits = (local_bits & 0x0000ffff0000ffffLL)
			+ ((local_bits >>16) & 0x0000ffff0000ffffLL);
		local_bits = (local_bits & 0x00000000ffffffffLL)
			+ ((local_bits >>32) & 0x00000000ffffffffLL);
		return local_bits;
	}

	/// @copydoc count_bit1()
	template<>
	std::size_t count_bit1(
		std::int64_t const in_bits)
	{
		return psyq::count_bit1(static_cast<std::uint64_t>(in_bits));
	}

	/** @brief �ŉ��ʃr�b�g����0���A�����鐔�𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŉ��ʃr�b�g����0���A�����鐔�B
	 */
	template<typename template_bits>
	std::size_t count_training_bit0(
		template_bits const in_bits)
	{
		return psyq::count_bit1((in_bits & (-in_bits)) - 1);
	}

	/** @brief 23�r�b�g�����ŁA�ŏ�ʃr�b�g����0���A�����鐔�𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŏ�ʃr�b�g����0���A�����鐔�B
	 */
	template<typename template_bits>
	std::size_t count_leading_bit0_23bits(
		template_bits const in_bits)
	{
		float const local_float(in_bits + 0.5f);
		return 149 - (psyq::get_float_bits(local_float) >> 23);
	}

	/** @brief 32�r�b�g�����ŁA�ŏ�ʃr�b�g����0���A�����鐔�𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŏ�ʃr�b�g����0���A�����鐔�B
	 */
	inline std::size_t count_leading_bit0_32bits(
		std::uint32_t const in_bits)
	{
		auto local_bits(in_bits);
		local_bits = local_bits | (local_bits >>  1);
		local_bits = local_bits | (local_bits >>  2);
		local_bits = local_bits | (local_bits >>  4);
		local_bits = local_bits | (local_bits >>  8);
		local_bits = local_bits | (local_bits >> 16);
		return psyq::count_bit1(~local_bits);
	}

	/** @brief 52�r�b�g�����ŁA�ŏ�ʃr�b�g����0���A�����鐔�𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŏ�ʃr�b�g����0���A�����鐔�B
	 */
	template<typename template_bits>
	std::size_t count_leading_bit0_52bits(
		template_bits const in_bits)
	{
		double const local_float(in_bits + 0.5);
		return 1074 - (psyq::get_float_bits(local_float) >> 52);
	}

	/** @brief 64�r�b�g�����ŁA�ŏ�ʃr�b�g����0���A�����鐔�𐔂���B

	    �ȉ��̃E�F�u�y�[�W���Q�l�ɂ����B
	    http://www.nminoru.jp/~nminoru/programming/bitcount.html
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŏ�ʃr�b�g����0���A�����鐔�B
	 */
	inline std::size_t count_leading_bit0_64bits(
		std::uint64_t const in_bits)
	{
		auto local_bits(in_bits);
		local_bits = local_bits | (local_bits >>  1);
		local_bits = local_bits | (local_bits >>  2);
		local_bits = local_bits | (local_bits >>  4);
		local_bits = local_bits | (local_bits >>  8);
		local_bits = local_bits | (local_bits >> 16);
		local_bits = local_bits | (local_bits >> 32);
		return psyq::count_bit1(~local_bits);
	}

	/** @brief �ŏ�ʃr�b�g����0���A�����鐔�𐔂���B
	    @param[in] in_bits  ������r�b�g�W���B
	    @return �ŏ�ʃr�b�g����0���A�����鐔�B
	 */
	template<typename template_bits>
	std::size_t count_leading_bit0(
		template_bits const in_bits)
	{
		return psyq::count_leading_bit0_23bits(in_bits)
			+ sizeof(template_bits) * 8 - 23;
	}

	/// @copydoc count_leading_bit0()
	template<>
	static std::size_t count_leading_bit0(
		std::uint32_t const in_bits)
	{
		return psyq::count_leading_bit0_52bits(in_bits) - 20;
	}

	/// @copydoc count_leading_bit0()
	template<>
	std::size_t count_leading_bit0(
		std::int32_t const in_bits)
	{
		return psyq::count_leading_bit0(static_cast<std::uint32_t>(in_bits));
	}

	/// @copydoc count_leading_bit0_()
	template<>
	std::size_t count_leading_bit0(
		std::uint64_t const in_bits)
	{
		if (in_bits < (std::uint64_t(1) << 52))
		{
			return psyq::count_leading_bit0_52bits(in_bits) + 12;
		}
		else
		{
			return psyq::count_leading_bit0_64bits(in_bits);
		}
	}

	/// @copydoc count_leading_bit0_()
	template<>
	std::size_t count_leading_bit0(
		std::int64_t const in_bits)
	{
		return psyq::count_leading_bit0(static_cast<std::uint64_t>(in_bits));
	}

} // namespace psyq

#endif // PSYQ_BIT_ALGORITHM_HPP_
