#ifndef PSYQ_SCENE_ACTION_HPP_
#define PSYQ_SCENE_ACTION_HPP_

#include <cmath>

namespace psyq
{
	template< typename > class scene_action;
}

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
template< typename t_stage >
class psyq::scene_action:
	public t_stage::event::action
{
	public: typedef psyq::scene_action< t_stage > this_type;
	public: typedef typename t_stage::event::action super_type;

	//-------------------------------------------------------------------------
	public: typedef t_stage stage;

	//-------------------------------------------------------------------------
	public: class set_scene_token;
	public: class remove_scene_token;
	public: class set_event_line;
	public: class set_scene_package;
	public: class set_scene_animation;
	public: class set_scene_model;
	public: class set_screen_camera;
	public: class set_screen_light;
	public: class set_time_scale;
	public: class remove_stage_element;

	//-------------------------------------------------------------------------
	public: class update_parameters:
		public t_stage::event::action::update_parameters
	{
		public: typedef update_parameters this_type;
		public: typedef typename t_stage::event::action::update_parameters
			super_type;

		public: update_parameters():
		super_type(),
		stage_(NULL)
		{
			// pass
		}

		/** @brief event-action��update�֐��Ŏg��������ݒ�B
		    @param[in,out] io_stage �X�V����stage�B
		    @param[in]     i_point  event����������point�B
		    @param[in]     i_time   event������������Ɍo�߂������ԁB
		 */
		public: void reset(
			t_stage&                                      io_stage,
			typename t_stage::event::action::point const& i_point,
			typename t_stage::real const                  i_time)
		{
			this->stage_ = &io_stage;
			this->super_type::reset(i_point, i_time);
		}

		public: t_stage* get_stage() const
		{
			return this->stage_;
		}

		private: t_stage* stage_;
	};
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_scene_token:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_scene_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// �ݒ肷��token����package-offset�l�B
		typename t_stage::event::package::offset token;

		/// screen����package-offset�l�B
		typename t_stage::event::package::offset screen;

		/// token�ɐݒ肷��time-scale����package-offset�l�B
		typename t_stage::event::package::offset scale;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_scene_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage��token��ݒ�B
			typename t_stage::hash::value const a_token_name(
				a_stage.event_.make_hash(a_parameters->token));
			typename t_stage::hash::value const a_screen_name(
				a_stage.event_.make_hash(a_parameters->screen));
			typename t_stage::token* const a_token(
				t_stage::hash::EMPTY != a_screen_name?
					a_stage.insert_screen_token(
						a_screen_name, a_token_name).get():
					a_stage.get_token(a_token_name).get());
			if (NULL != a_token)
			{
				// token��time-scale��ݒ�B
				typename t_stage::hash::value const a_scale_name(
					a_stage.event_.make_hash(a_parameters->scale));
				if (t_stage::hash::EMPTY != a_scale_name)
				{
					a_token->time_scale_ = a_stage.event_.find_scale(
						a_scale_name);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token����菜��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::remove_scene_token:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::remove_scene_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// ��菜��token����package-offset�l�B
		typename t_stage::event::package::offset token;

		/// screen����package-offset�l�B
		typename t_stage::event::package::offset screen;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("remove_scene_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			typename t_stage::hash::value const a_token_name(
				a_stage.event_.make_hash(a_parameters->token));
			typename t_stage::hash::value const a_screen_name(
				a_stage.event_.make_hash(a_parameters->screen));
			if (t_stage::hash::EMPTY != a_screen_name)
			{
				// screen����token����菜���B
				a_stage.remove_screen_token(a_screen_name, a_token_name);
			}
			else
			{
				// stage�Ƃ��ׂĂ�screen����token����菜���B
				a_stage.remove_token(a_token_name);
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief event-line��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_event_line:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_event_line this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// �ݒ肷��event-line����package-offset�l�B
		typename t_stage::event::package::offset line;

		/// event-line�ɐݒ肷��event�z�񖼂�package-offset�l�B
		typename t_stage::event::package::offset points;

		/// event-line�ɐݒ肷��time-scale����package-offset�l�B
		typename t_stage::event::package::offset scale;

		/// event-line�ɐݒ肷�鎞�ԁB
		typename t_stage::real time;

		/// event-line�ɐݒ肷�鎞�Ԃ̊�_�B
		typename t_stage::hash::value origin;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_event_line");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����event-line���擾�B
			typename t_stage::event::line* const a_line(
				a_stage.event_.get_line(
					a_stage.event_.make_hash(a_parameters->line)));
			if (NULL != a_line)
			{
				// event-line���������B
				typename t_stage::hash::value const a_points_name(
					a_stage.event_.make_hash(a_parameters->points));
				if (t_stage::hash::EMPTY != a_points_name)
				{
					a_line->reset(
						a_stage.event_.get_package(), a_points_name);
				}

				// time-scale�Ȃ���event-line�Ɏ��Ԃ�ݒ�B
				typename t_stage::event::line::scale::shared_ptr a_scale;
				a_scale.swap(a_line->scale_);
				int a_origin(a_parameters->origin);
				switch (a_origin)
				{
					case 0:
					a_origin = SEEK_SET;
					goto SEEK;

					case 1:
					a_origin = SEEK_CUR;
					goto SEEK;

					case 2:
					a_origin = SEEK_END;
					goto SEEK;

					SEEK:
					a_line->seek(a_parameters->time, a_origin);
					break;
				}

				// event-line��time-scale��ݒ�B
				typename t_stage::hash::value const a_scale_name(
					a_stage.event_.make_hash(a_parameters->scale));
				if (t_stage::hash::EMPTY != a_scale_name)
				{
					// �V����time-scale��p�ӁB
					a_scale = a_stage.event_.find_scale(a_scale_name);
				}
				a_line->scale_.swap(a_scale);
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_scene_package:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_scene_package this_type;

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_scene_package");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// stage��package��p�ӁB
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename t_stage::hash::value const a_package_name(
			a_stage.event_.make_hash(i_update.get_point()->integer));
		if (t_stage::hash::EMPTY != a_package_name)
		{
			a_stage.get_package(a_package_name);
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��animation��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_scene_animation:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_scene_animation this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// animation��ݒ肷��token����package-offset�l�B
		typename t_stage::event::package::offset token;

		/// animation������scene-package����package-offset�l�B
		typename t_stage::event::package::offset package;

		typename t_stage::event::package::offset flags;
		typename t_stage::real start;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_scene_animation");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����scene-package���擾�B
			psyq::scene_package* const a_package(
				a_stage.find_package(
					a_stage.event_.make_hash(a_parameters->package)).get());
			if (NULL != a_package)
			{
				// stage����token���擾���Aanimation��ݒ�B
				typename t_stage::token* const a_token(
					a_stage.find_token(
						a_stage.event_.make_hash(a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_animation(
						a_token->scene_,
						*a_package,
						t_stage::event::line::scale::get_current(
							a_token->time_scale_, i_update.get_time()));
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_scene_model:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_scene_model this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// model��ݒ肷��token����package-offset�l�B
		typename t_stage::event::package::offset token;

		/// model������scene-package����package-offset�l�B
		typename t_stage::event::package::offset package;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_scene_model");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����scene-package���擾�B
			psyq::scene_package* const a_package(
				a_stage.find_package(
					a_stage.event_.make_hash(a_parameters->package)).get());
			if (NULL != a_package)
			{
				// stage����token���擾���Amodel��ݒ�B
				typename t_stage::token* const a_token(
					a_stage.find_token(
						a_stage.event_.make_hash(a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_model(a_token->scene_, *a_package);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief screen��camera��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_screen_camera:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_screen_camera this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// camera��ݒ肷��screen����package-offset�l�B
		typename t_stage::event::package::offset screen;

		/// camera�Ɏg��token����package-offset�l�B
		typename t_stage::event::package::offset camera_token;

		/// camera�Ɏg��node����package-offset�l�B
		typename t_stage::event::package::offset camera_node;

		/// focus�Ɏg��token����pacakge-offset�l�B
		typename t_stage::event::package::offset focus_token;

		/// focus�Ɏg��node����package-offset�l�B
		typename t_stage::event::package::offset focus_node;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_screen_camera");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����screen���擾�B
			typename t_stage::screen* const a_screen(
				a_stage.get_screen(
					a_stage.event_.make_hash(a_parameters->screen)).get());
			if (NULL != a_screen)
			{
				typename t_stage::hash::value const a_camera_token(
					a_stage.event_.make_hash(a_parameters->camera_token));
				if (t_stage::hash::EMPTY != a_camera_token)
				{
					// screen��camera��ݒ�B
					typename t_stage::token::shared_ptr const& a_token(
						a_stage.find_token(a_camera_token));
					typename t_stage::event::string const a_name(
						a_stage.event_.make_string(a_parameters->camera_node));
					psyq_extern::scene_node const* const a_node(
						a_screen->set_camera(
							a_token, a_name.empty()? NULL: a_name.c_str()));
					if(NULL == a_node)
					{
						a_screen->remove_camera();
					}
				}
				typename t_stage::hash::value const a_focus_token(
					a_stage.event_.make_hash(a_parameters->focus_token));
				if (t_stage::hash::EMPTY != a_focus_token)
				{
					// screen�ɏœ_��ݒ�B
					typename t_stage::token::shared_ptr const& a_token(
						a_stage.find_token(a_focus_token));
					typename t_stage::event::string const a_name(
						a_stage.event_.make_string(a_parameters->focus_node));
					psyq_extern::scene_node const* const a_node(
						a_screen->set_focus(
							a_token, a_name.empty()? NULL: a_name.c_str()));
					if (NULL == a_node)
					{
						a_screen->remove_focus();
					}
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief screen��light��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_screen_light:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_screen_light this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// light��ݒ肷��screen����package-offset�l�B
		typename t_stage::event::package::offset screen;

		/// screen�ɐݒ肷��light����package-offset�l�B
		typename t_stage::event::package::offset light;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_screen_light");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����screen���擾�B
			typename t_stage::screen* const a_screen(
				a_stage.get_screen(
					a_stage.event_.make_hash(a_parameters->screen)).get());
			if (NULL != a_screen)
			{
				typename t_stage::hash::value const a_light_name(
					a_stage.event_.make_hash(a_parameters->light));
				if (t_stage::hash::EMPTY != a_light_name)
				{
					// stage����light-token���������Acamera�ɐݒ�B
					a_screen->light_ = a_stage.find_token(a_light_name);
				}
			}
		}
	}

};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief time-scale��ݒ肷��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::set_time_scale:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::set_time_scale this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// time-scale����package-offset�l�B
		typename t_stage::event::package::offset scale;

		/// ���time-scale����package-offset�l�B
		typename t_stage::event::package::offset super;

		/// time-scale����`��Ԃ���Ƃ���frame���B
		typename t_stage::event::package::offset frame;

		/// time-scale����`��Ԃ���Ƃ��̊J�nscale�l�B
		/// NAN�̏ꍇ�́A���ݒl���J�nscale�l�Ƃ��Ďg���B
		typename t_stage::real start;

		/// time-scale����`��Ԃ���Ƃ��̏I��scale�l�B
		typename t_stage::real end;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("set_time_scale");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����time-scale���擾�B
			typename t_stage::event::line::scale::shared_ptr const a_scale(
				a_stage.event_.get_scale(
					a_stage.event_.make_hash(a_parameters->scale)));
			if (NULL != a_scale.get())
			{
				// ���`���scale���������B
				typename t_stage::event::line::scale::lerp const
					a_lerp(
						a_parameters->frame,
						this_type::is_nan(a_parameters->start)?
							a_scale->get_current(): a_parameters->start,
						a_parameters->end);
				typename t_stage::hash::value const a_super_name(
					a_stage.event_.make_hash(a_parameters->super));
				if (t_stage::hash::EMPTY != a_super_name)
				{
					// scale�l�Ə��scale��ݒ�B
					a_scale->reset(
						a_lerp, a_stage.event_.find_scale(a_super_name));
				}
				else
				{
					// scale�l�݂̂�ݒ�B
					a_scale->reset(a_lerp);
				}
			}
		}
	}

	private: static bool is_nan(float const i_value)
	{
#ifdef _WIN32
		return 0 != ::_isnan(i_value);
#else
		return std::isnan(i_value);
#endif // _WIN32
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene�Ŏg���Ă���v�f����菜��event�B
template< typename t_stage >
class psyq::scene_action< t_stage >::remove_stage_element:
	public psyq::scene_action< t_stage >
{
	public: typedef psyq::scene_action< t_stage > super_type;
	public: typedef typename super_type::remove_stage_element this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		/// ��菜���v�f�̌^����package-offset�l�B
		typename t_stage::event::package::offset kind;

		/// ��菜���v�f����package-offset�l�B
		typename t_stage::event::package::offset name;
	};

	//-------------------------------------------------------------------------
	public: static typename t_stage::hash::value get_hash()
	{
		return t_stage::hash::generate("remove_stage_element");
	}

	//-------------------------------------------------------------------------
	public: virtual void update(
		typename t_stage::event::action::update_parameters const& i_update)
	{
		// package����������擾�B
		typename super_type::update_parameters const& a_update(
			static_cast< typename super_type::update_parameters const& >(
				i_update));
		t_stage& a_stage(*a_update.get_stage());
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.template get_value<
				typename this_type::parameters >(
					i_update.get_point()->integer));
		if (NULL != a_parameters)
		{
			// stage����time-scale���擾�B
			typename t_stage::hash::value const a_name(
				a_stage.event_.make_hash(a_parameters->name));
			if (t_stage::hash::EMPTY != a_name)
			{
				typename t_stage::hash::value const a_kind(
					a_stage.event_.make_hash(a_parameters->kind));
				if (t_stage::hash::generate("scene_token") == a_kind)
				{
					a_stage.remove_token(a_name);
				}
				else if (t_stage::hash::generate("scene_screen") == a_kind)
				{
					a_stage.screens_.erase(a_name);
				}
				else if (t_stage::hash::generate("event_line") == a_kind)
				{
					a_stage.event_.lines_.erase(a_name);
				}
				else if (t_stage::hash::generate("time_scale") == a_kind)
				{
					a_stage.remove_scale(a_name);
				}
			}
		}
	}
};

#endif // !PSYQ_SCENE_ACTION_HPP_
