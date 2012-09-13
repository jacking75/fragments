#ifndef PSYQ_SCENE_EVENT_ACTION_HPP_
#define PSYQ_SCENE_EVENT_ACTION_HPP_

//#include <psyq/scene/event_line.hpp>

namespace psyq
{
	class scene_world;
	template< typename, typename, typename > class event_action;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// event-action�̊��class�B
template< typename t_hash, typename t_real, typename t_string >
class psyq::event_action
{
	typedef psyq::event_action< t_hash, t_real, t_string > this_type;

	public: typedef t_hash hash; ///< event���ɂŎg���Ă���hash�֐��B
	public: typedef t_real real; ///< event���ɂŎg���Ă�������̌^�B
	public: typedef t_string string; ///< ������̌^�B
	public: typedef PSYQ_SHARED_PTR< this_type > shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type > weak_ptr;
	public: typedef PSYQ_SHARED_PTR< this_type const > const_shared_ptr;
	public: typedef PSYQ_WEAK_PTR< this_type const > const_weak_ptr;

	// �p�ӂ���Ă���event-action�B
	public: class load_package;
	public: class load_token;
	public: class unload_token;
	public: class set_token_animation;
	public: class set_token_model;
	public: class set_section_camera;
	public: class set_section_light;
	public: class set_event_line;
	public: class set_time_scale;

	protected: event_action() {}
	public: virtual ~event_action() {}

	/** event-action��K�p�B
	    @param[in,out] io_world �K�p�Ώۂ�scene-world�B
	    @param[in]     i_point  event�𔭐�������point�B
	    @param[in]     i_time   event��K�p�������ƂɌo�߂��鎞�ԁB
	 */
	public: virtual void apply(
		psyq::scene_world&                         io_world,
		psyq::event_point< t_hash, t_real > const& i_point,
		t_real const                               i_time) = 0;
};

#endif // !PSYQ_SCENE_EVENT_ACTION_HPP_
