#ifndef PSYQ_LERP_HPP_
#define PSYQ_LERP_HPP_

namespace psyq
{
	template< typename, typename > class lerp;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief �C�ӌ^�̒l�̐��`���animation�B
	@tparam t_value ���`��Ԃ���l�̌^�B
	@tparam t_time  ���`��Ԃ��鎞�Ԃ̌^�B
 */
template< typename t_value, typename t_time >
class psyq::lerp
{
	typedef psyq::lerp< t_value, t_time > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_value value_type; ///< ���`��Ԃ���l�̌^�B
	public: typedef t_time time;        ///< ���`��Ԃ̎��Ԃ̌^�B

	//-------------------------------------------------------------------------
	/** @param i_current �����l�B
	 */
	public: explicit lerp(t_value const& i_current = t_value(0)):
	value_diff_(0),
	end_value_(i_current),
	time_diff_(1),
	rest_time_(0)
	{
		// pass
	}

	/**	@param[in] i_time  �I���܂ł̎��ԁB
	    @param[in] i_start ���`��Ԃ̊J�n�l�B
	    @param[in] i_end   ���`��Ԃ̏I���l�B
	 */
	public: lerp(
		t_time const&  i_time,
		t_value const& i_start,
		t_value const& i_end):
	end_value_(i_end)
	{
		if (t_time(0) < i_time)
		{
			this->value_diff_ = i_end - i_start;
			this->rest_time_ = i_time;
			this->time_diff_ = i_time;
		}
		else
		{
			this->value_diff_ = t_value(0);
			this->rest_time_ = t_time(0);
			this->time_diff_ = t_time(1);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief ���`��ԂŏI�����ԂɒB����������B
	 */
	public: bool is_end() const
	{
		return this->rest_time_ <= t_time(0);
	}

	/** @brief ���`��Ԃ̌��ݒl���擾�B
	 */
	public: t_value current() const
	{
		return this->end_value_ - static_cast< t_value >(
			(this->value_diff_ * this->rest_time_) / this->time_diff_);
	}

	//-------------------------------------------------------------------------
	/** @brief ���`��Ԃ̎��Ԃ�i�߂�B
		@param[in] i_time �ǂꂾ�����Ԃ�i�߂邩�B
	 */
	public: void update(t_time const& i_time)
	{
		if (i_time < this->rest_time_)
		{
			// ���ݒl���X�V�B
			this->rest_time_ -= i_time;
		}
		else
		{
			// �I�����Ԃ��߂����̂ŏI���l�̂܂܁B
			this->rest_time_ = t_time(0);
		}
	}

	//-------------------------------------------------------------------------
	/** @brief ���`��Ԃ̍Đݒ�B
	    @param[in] i_time  �I���܂ł̎��ԁB
	    @param[in] i_start ���`��Ԃ̊J�n�l�B
	    @param[in] i_end   ���`��Ԃ̏I���l�B
	 */
	public: void reset(
		t_time const&  i_time,
		t_value const& i_start,
		t_value const& i_end)
	{
		new(this) this_type(i_time, i_start, i_end);
	}

	/** @brief ���`��Ԃ̍Đݒ�B
	    @param[in] i_time �I���܂ł̎��ԁB
	    @param[in] i_end  ���`��Ԃ̏I���l�B
	 */
	public: void reset(t_time const&  i_time, t_value const& i_end)
	{
		this->reset(i_time, this->current(), i_end);
	}

	public: void reset(t_value const& i_current)
	{
		new(this) this_type(i_current);
	}

	//-------------------------------------------------------------------------
	private: t_value value_diff_; ///< �J�n�l�ƏI���l�̍��B
	private: t_value end_value_;  ///< �I���l
	private: t_time  time_diff_;  ///< �J�n���ԂƏI�����Ԃ̍��B
	private: t_time  rest_time_;  ///< �c�莞�ԁB
};

#endif // PSYQ_LERP_HPP_
