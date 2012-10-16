#ifndef PSYQ_SCENE_EVENT_LINE_HPP_
#define PSYQ_SCENE_EVENT_LINE_HPP_

//#include <psyq/layered_scale.hpp>
//#include <psyq/scene/event_package.hpp>
//#include <psyq/scene/event_action.hpp>

namespace psyq
{
	template< typename, typename > class event_line;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event���Ԏ��̊Ǘ��B
    @tparam t_hash event-package�Ŏg���Ă���hash�֐��B
    @tparam t_real event-package�Ŏg���Ă�������̌^�B
 */
template< typename t_hash, typename t_real >
class psyq::event_line
{
	public: typedef psyq::event_line< t_hash, t_real > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash;
	public: typedef t_real real;
	public: typedef typename t_hash::value integer;
	public: typedef psyq::layered_scale<
		t_real, typename this_type::integer, this_type >
			scale;
	public: typedef typename psyq::event_package< t_hash > package;
	private: typedef typename psyq::event_action< t_hash, t_real >::point
		point;

	//-------------------------------------------------------------------------
	public: event_line():
	first_point_(NULL),
	last_point_(NULL),
	cache_time_(0),
	rest_time_(0)
	{
		// pass
	}

	/** @param[in] i_package event-point�z�񂪕ۑ�����Ă���event-package�B
	    @param[in] i_points  event-point�z��̖��Ohash�l�B
	 */
	public: event_line(
		typename this_type::package::const_shared_ptr const& i_package,
		typename t_hash::value const                         i_points)
	{
		new(this) this_type();
		this->reset(i_package, i_points);
	}

	//-------------------------------------------------------------------------
	/** @brief �l�������B
	    @param[in,out] ��������ΏہB
	 */
	public: void swap(this_type& io_target)
	{
		this->scale_.swap(io_target.scale_);
		this->package_.swap(io_target.package_);
		std::swap(this->first_point_, io_target.first_point_);
		std::swap(this->last_point_, io_target.last_point_);
		std::swap(this->cache_time_, io_target.cache_time_);
		std::swap(this->rest_time_, io_target.rest_time_);
	}

	//-------------------------------------------------------------------------
	/** @brief event-line���������B
	    @param[in] i_package event-point�z�񂪕ۑ�����Ă���event-package�B
	    @param[in] i_points  event-point�z��̖��Ohash�l�B
	    @retval !=false �����B
	    @retval ==false ���s�B
	 */
	public: bool reset(
		typename this_type::package::const_shared_ptr const& i_package,
		typename t_hash::value const                         i_points)
	{
		// event-package���擾�B
		typename this_type::package const* const a_package(i_package.get());
		if (NULL != a_package)
		{
			// ���ڂ���event-point�z��̐擪�ʒu���擾�B
			typename this_type::point const* const a_first_point(
				a_package->template find_value< typename this_type::point >(
					i_points));
			if (NULL != a_first_point)
			{
				this->package_ = i_package;

				// event-line���������B
				this->first_point_ = a_first_point;
				this->last_point_ = a_first_point;
				this->cache_time_ = 0;
				this->rest_time_ = a_first_point->time;
				return true;
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
		this->seek(1, i_time, i_origin);
	}

	/** @brief �Đ��J�n���Ԃ�C�ӂ̈ʒu�ɐݒ�B
	    @param[in] i_fps    1�b�������frame���B
	    @param[in] i_count  �i�߂�frame���B
	    @param[in] i_origin ��ƂȂ鎞�ԁBC�W��libraly��SEEK�萔���g���B
	 */
	public: void seek(
		t_real const i_fps,
		t_real const i_count,
		int const    i_origin)
	{
		if (NULL != this->last_point_)
		{
			t_real const a_time(
				i_fps < 0 || 0 < i_fps?
					this_type::scale::get_current(this->scale_, i_count) / i_fps:
					0);
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

	/** @brief 0����Ɏ��Ԃ�ݒ�B
	    @param[in] i_time �J�n���ԁB
	 */
	private: void seek_front(t_real const i_time)
	{
		this->last_point_ = this->first_point_;
		this->cache_time_ = i_time;
		this->rest_time_ = this->first_point_->time;
	}

	//-------------------------------------------------------------------------
	/** @brief event-line����~���Ă��邩����B
	 */
	public: bool is_stop() const
	{
		return NULL == this->last_point_ || (
			this->rest_time_ <= 0 && this_type::is_last(this->last_point_));
	}

	//-------------------------------------------------------------------------
	/** @brief event-package���擾�B
	 */
	public:
	typename this_type::package::const_shared_ptr const& get_package() const
	{
		return this->package_;
	}

	//-------------------------------------------------------------------------
	/** @brief ��������event��o�^�B
	    @param[in,out] io_points ��������event-point��o�^����container�B
	 */
	public: template< typename t_container >
	void _dispatch(t_container& io_points)
	{
		if (NULL != this->last_point_)
		{
			// �i�߂鎞�Ԃ�����B
			t_real a_cache_time(this->cache_time_);
			t_real a_rest_time(this->rest_time_);
			if (a_cache_time < 0)
			{
				// cache-time�����Ȃ̂ŁA���Ԃ������߂��B
				a_cache_time +=
					this->get_dispatch_time(this->last_point_) - a_rest_time;
				if (0 <= a_cache_time)
				{
					// 0����ɊJ�n���Ԃ�ݒ�B
					this->seek_front(a_cache_time);
				}
				else
				{
					// 0���O�Ɋ����߂����B
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
				io_points.insert(
					typename t_container::value_type(a_cache_time, i));
				a_cache_time -= a_rest_time;
				PSYQ_ASSERT(0 <= a_cache_time);
			}
			this->cache_time_ = 0;
		}
	}

	/** @brief ���Ԃ�i�߂�B
	    @param[in] i_time �i�߂鎞�ԁB
	 */
	private: void forward_time(t_real const i_time)
	{
		typename this_type::point const* a_point(this->last_point_);
		if (0 <= i_time && NULL != a_point)
		{
			t_real a_rest_time(this->rest_time_ - i_time);
			while (a_rest_time <= 0)
			{
				if (this_type::is_last(a_point))
				{
					a_rest_time = 0;
					break;
				}
				else
				{
					++a_point;
					a_rest_time += a_point->time;
				}
			}
			this->rest_time_ = a_rest_time;
			this->last_point_ = a_point;
		}
	}

	//-------------------------------------------------------------------------
	/** @brief �J�n���Ԃ���Ƃ���event�������Ԃ��擾�B
	    @param[in] i_point �������Ԃ𒲂ׂ�event�����_�B
	 */
	private: t_real get_dispatch_time(
		typename this_type::point const* const i_point)
	const
	{
		typename this_type::point const* a_point(this->first_point_);
		t_real a_time(0);
		if (NULL != a_point)
		{
			for (;;)
			{
				a_time += a_point->time;
				if (i_point == a_point || this_type::is_last(a_point))
				{
					break;
				}
				else
				{
					++a_point;
				}
			}
		}
		return a_time;
	}

	//-------------------------------------------------------------------------
	/** @brief �Ō��event������B
	 */
	private: static bool is_last(
		typename this_type::point const* const i_point)
	{
		PSYQ_ASSERT(NULL != i_point);
		return t_hash::EMPTY == i_point->type && 0 == i_point->integer;
	}

	//-------------------------------------------------------------------------
	/// ���Ԃ̏k�ڗ��B
	public: typename this_type::scale::shared_ptr scale_;

	/// �Q�Ƃ��Ă���event-package�B
	private: typename this_type::package::const_shared_ptr package_;

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

#endif // !PSYQ_SCENE_EVENT_LINE_HPP_
