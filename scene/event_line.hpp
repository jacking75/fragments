#ifndef PSYQ_EVENT_LINE_HPP_
#define PSYQ_EVENT_LINE_HPP_

namespace psyq
{
	template< typename, typename > struct event_point;
	template< typename, typename > class event_line;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_hash, typename t_real = float >
struct psyq::event_point
{
	typename t_hash::value type; ///< event�̎�ʁB
	t_real                 time; ///< event����������܂ł̎��ԁB
	union
	{
		typename t_hash::value integer; ///< �����^�̈����B
		t_real                 real;    ///< ���������_�^�̈����B
	};
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_hash, typename t_real = float >
class psyq::event_line
{
	typedef psyq::event_line< t_hash, t_real > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef typename t_hash::value integer;
	public: typedef psyq::layered_scale< t_real, typename this_type::integer >
		time_scale;
	public: typedef psyq::event_point< t_hash, t_real > point;
	public: typedef typename psyq::event_item< t_hash >::archive archive;

	//-------------------------------------------------------------------------
	public: event_line():
	first_point_(NULL),
	last_point_(NULL),
	cache_time_(0),
	rest_time_(0)
	{
		// pass
	}

	/** @param[in] i_archive event-point�z�񂪕ۑ�����Ă��鏑�ɁB
	    @param[in] i_points  event-point�z��̖��Ohash�l�B
	 */
	public: event_line(
		PSYQ_SHARED_PTR< typename this_type::archive const > const& i_archive,
		typename t_hash::value const                                i_points)
	{
		new(this) this_type();
		this->reset(i_archive, i_points);
	}

	//-------------------------------------------------------------------------
	public: void swap(this_type& io_target)
	{
		this->time_scale_.swap(io_target.time_scale_);
		this->archive_.swap(io_target.archive_);
		std::swap(this->first_point_, io_target.first_point_);
		std::swap(this->last_point_, io_target.last_point_);
		std::swap(this->cache_time_, io_target.cache_time_);
		std::swap(this->rest_time_, io_target.rest_time_);
	}

	//-------------------------------------------------------------------------
	/** @brief event-line���������B
	    @param[in] i_archive event-point�z�񂪕ۑ�����Ă��鏑�ɁB
	    @param[in] i_name    event-point�z��̖��Ohash�l�B
	 */
	public: bool reset(
		PSYQ_SHARED_PTR< typename this_type::archive const > const& i_archive,
		typename t_hash::value const                                i_name)
	{
		// ���ɂ��擾�B
		typedef psyq::event_item< t_hash > item;
		typename this_type::archive const* const a_archive(i_archive.get());
		if (NULL != a_archive)
		{
			// ���ɂ��獀�ڂ��擾�B
			item const* const a_item(item::find(*a_archive, i_name));
			if (NULL != a_item)
			{
				// ���ڂ���event-point�z��̐擪�ʒu���擾�B
				typename this_type::point const* const a_first_point(
					////item::get_address< this_type::point >(
					////	*a_archive, a_item->begin));
					static_cast< typename this_type::point const* >(
						item::get_address(*a_archive, a_item->begin)));
				if (NULL != a_first_point)
				{
					this->archive_ = i_archive;

					// event-line���������B
					this->first_point_ = a_first_point;
					this->last_point_ = a_first_point;
					this->cache_time_ = 0;
					this->rest_time_ = a_first_point->time;
					return true;
				}
			}
		}
		return false;
	}

	//-------------------------------------------------------------------------
	/** @brief �Đ��J�n���Ԃ�C�ӂ̈ʒu�ɐݒ�B
	    @param[in] i_time   �����̑��Ύ��ԁB
	    @param[in] i_origin ��ƂȂ鎞�ԁBC�W��libraly��SEEK�萔���g���B
	 */
	public: void seek(t_real const i_time, int const i_origin)
	{
		if (NULL != this->last_point_)
		{
			t_real const a_time(
				NULL != this->time_scale_.get()?
					i_time * this->time_scale_->get_scale(): i_time);
			switch (i_origin)
			{
				case SEEK_SET: // �擪������ԁB
				this->seek_front(a_time);
				break;

				case SEEK_END: // ����������ԁB
				this->seek_front(a_time + this->get_dispatch_time(NULL));
				break;

				case SEEK_CUR: // ���݂�����ԁB
				this->cache_time_ += a_time;
				break;
			}
		}
	}

	//-------------------------------------------------------------------------
	/** @brief ��������event��o�^�B
	    @param[in,out] io_container ��������event��o�^����container�B
	 */
	public: template< typename t_container >
	void dispatch(t_container& io_container)
	{
		if (NULL == this->last_point_)
		{
			return;
		}

		t_real a_cache_time(this->cache_time_);
		t_real a_rest_time(this->rest_time_);
		if (a_cache_time < 0)
		{
			// ���Ԃ������߂��B
			a_cache_time +=
				this->get_dispatch_time(this->last_point_) - a_rest_time;
			if (0 <= a_cache_time)
			{
				this->seek_front(a_cache_time);
			}
			else
			{
				this->cache_time_ = a_cache_time;
				return;
			}
		}

		// ����̍X�V�Ŕ�������event�̐擪�ʒu�Ɩ����ʒu������B
		typename this_type::point const* const a_begin(this->last_point_);
		this->forward_time(a_cache_time);
		typename this_type::point const* const a_end(this->last_point_);

		// ����̔�������event��container�ɓo�^�B
		for (
			typename this_type::point const* i = a_begin;
			a_end != i;
			++i, a_rest_time = i->time)
		{
			io_container.insert(
				typename t_container::value_type(a_cache_time, i));
			a_cache_time -= a_rest_time;
			PSYQ_ASSERT(0 <= a_cache_time);
		}
		this->cache_time_ = 0;
	}

	//-------------------------------------------------------------------------
	/** @brief event-line����~���Ă��邩����B
	 */
	public: bool is_stop() const
	{
		return NULL == this->last_point_ || (
			this->rest_time_ <= 0 &&
			this_type::is_last(this->last_point_));
	}

	//-------------------------------------------------------------------------
	public: PSYQ_SHARED_PTR< typename this_type::archive const > const&
	get_archive() const
	{
		return this->archive_;
	}

	//-------------------------------------------------------------------------
	/** @brief ���Ԃ�i�߂�B
	    @param[in] i_time �i�߂鎞�ԁB
	 */
	private: void forward_time(t_real const i_time)
	{
		typename this_type::point const* a_event(this->last_point_);
		if (0 <= i_time && NULL != a_event)
		{
			t_real a_rest_time(this->rest_time_ - i_time);
			while (a_rest_time <= 0)
			{
				if (this_type::is_last(a_event))
				{
					a_rest_time = 0;
					break;
				}
				else
				{
					++a_event;
					a_rest_time += a_event->time;
				}
			}
			this->rest_time_ = a_rest_time;
			this->last_point_ = a_event;
		}
	}

	//-------------------------------------------------------------------------
	private: void seek_front(t_real const i_time)
	{
		this->last_point_ = this->first_point_;
		this->cache_time_ = i_time;
		this->rest_time_ = this->first_point_->time;
	}

	//-------------------------------------------------------------------------
	/** @brief �J�n���Ԃ���Ƃ���event�������Ԃ��擾�B
	 */
	private: t_real get_dispatch_time(
		typename this_type::point const* const i_event)
	const
	{
		typename this_type::point const* a_event(this->first_point_);
		t_real a_time(0);
		if (NULL != a_event)
		{
			for (;;)
			{
				a_time += a_event->time;
				if (i_event == a_event || this_type::is_last(a_event))
				{
					break;
				}
				else
				{
					++a_event;
				}
			}
		}
		return a_time;
	}

	//-------------------------------------------------------------------------
	/** @brief �Ō��event������B
	 */
	private: static bool is_last(
		typename this_type::point const* const i_event)
	{
		PSYQ_ASSERT(NULL != i_event);
		return t_hash::EMPTY == i_event->type &&
			t_hash::EMPTY == i_event->integer;
	}

	//-------------------------------------------------------------------------
	public: typename this_type::time_scale::shared_ptr time_scale_;

	/// �Q�Ƃ��Ă���event���ɁB
	private: PSYQ_SHARED_PTR< typename this_type::archive const > archive_;

	/// event�z��̐擪�ʒu�B
	private: typename this_type::point const* first_point_;

	/// ���łɔ�������event�z��̖����ʒu�B
	private: typename this_type::point const* last_point_;

	/// �X�V���鎞�ԁB
	private: t_real cache_time_;

	/// ����event����������܂ł̎��ԁB
	private: t_real rest_time_;
};

namespace std
{
	template< typename t_hash, typename t_real >
	void swap(
		psyq::event_line< t_hash, t_real >& io_left,
		psyq::event_line< t_hash, t_real >& io_right)
	{
		io_left.swap(io_right);
	}
}

#endif // !PSYQ_EVENT_LINE_HPP_
