#ifndef PSYQ_SCENE_EVENT_ACTION_HPP_
#define PSYQ_SCENE_EVENT_ACTION_HPP_

//#include <psyq/scene/event_line.hpp>

namespace psyq
{
	template< typename, typename > class event_action;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/** @brief event-action�̊��class�B
    @tparam t_hash event�Ŏg��hash�֐��B
    @tparam t_real event�Ŏg�������̌^�B
 */
template< typename t_hash, typename t_real >
class psyq::event_action
{
	public: typedef psyq::event_action< t_hash, t_real > this_type;

	//-------------------------------------------------------------------------
	public: typedef t_hash hash; ///< event���ɂŎg���Ă���hash�֐��B
	public: typedef t_real real; ///< event���ɂŎg���Ă�������̌^�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	//-------------------------------------------------------------------------
	public: class update_parameters
	{
		public: update_parameters():
		point_(NULL),
		time_(0)
		{
			// pass
		}

		public: void reset(
			psyq::event_point< t_hash, t_real > const& i_point,
			t_real const                               i_time)
		{
			this->point_ = &i_point;
			this->time_ = i_time;
		}

		public: psyq::event_point< t_hash, t_real > const* get_point() const
		{
			return this->point_;
		}

		public: t_real get_time() const
		{
			return this->time_;
		}

		private: psyq::event_point< t_hash, t_real > const* point_;
		private: t_real                                     time_;
	};

	//-------------------------------------------------------------------------
	public: virtual ~event_action() {}

	/** event-action��K�p�B
		@param[in] i_update �K�p��������B
	 */
	public: virtual void update(
		typename this_type::update_parameters const& i_update) = 0;
};

#endif // !PSYQ_SCENE_EVENT_ACTION_HPP_
