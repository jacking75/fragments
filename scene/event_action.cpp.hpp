#ifndef PSYQ_SCENE_EVENT_ACTION_CPP_HPP_
#define PSYQ_SCENE_EVENT_ACTION_CPP_HPP_

#include <cmath>
//#include <psyq/scene/scene_stage.hpp>

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��p�ӂ���event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::load_package:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::load_package this_type;

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("load_package");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// stage��package��p�ӁB
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		a_stage.get_package(
			a_stage.event_.replace_hash(i_apply.point_.integer));
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��p�ӂ���event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::load_token:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::load_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset section; ///< section���̏���offset�l�B
		typename super_type::stage::event::item::offset token;   ///< �ǉ�����token���̏���offset�l�B
		typename super_type::stage::event::item::offset scale;   ///< �ݒ肷��time-scale���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("load_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// section��token��stage�ɗp�ӁB
			typename super_type::stage::token* const a_token(
				a_stage.get_token(
					a_stage.event_.replace_hash(a_parameters->token),
					a_stage.event_.replace_hash(a_parameters->section)).get());
			if (NULL != a_token)
			{
				// token��time-scale��ݒ�B
				typename super_type::stage::event::hash::value const a_scale(
					a_stage.event_.replace_hash(a_parameters->scale));
				if (super_type::stage::event::hash::EMPTY != a_scale)
				{
					a_token->time_scale_ = a_stage.event_.get_scale(a_scale);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token���폜����event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::unload_token:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::unload_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset section; ///< section���̏���offset�l�B
		typename super_type::stage::event::item::offset token;   ///< �폜����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("unload_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			typename super_type::stage::event::hash::value const a_token(
				a_stage.event_.replace_hash(a_parameters->token));
			typename super_type::stage::event::hash::value const a_section(
				a_stage.event_.replace_hash(a_parameters->section));
			if (super_type::stage::event::hash::EMPTY != a_section)
			{
				// section����token���폜�B
				a_stage.erase_token(a_token, a_section);
			}
			else
			{
				// ���ׂĂ�section����token���폜�B
				a_stage.erase_token(a_token);
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��animation��ݒ肷��event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_token_animation:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::set_token_animation this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset token;
		typename super_type::stage::event::item::offset package;
		typename super_type::stage::event::item::offset flags;
		typename super_type::stage::event::real         start;
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("set_token_animation");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// stage����animation-package���擾�B
			psyq::scene_package* const a_package(
				a_stage.get_package(
					a_stage.event_.replace_hash(
						a_parameters->package)).get());
			if (NULL != a_package)
			{
				// stage����token���擾���Aanimation��ݒ�B
				typename super_type::stage::token* const a_token(
					a_stage.get_token(
						a_stage.event_.replace_hash(
							a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_animation(
						a_token->scene_,
						*a_package,
						super_type::stage::event::line::scale::get_scale(
							a_token->time_scale_, i_apply.time_));
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_token_model:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::set_token_model this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset token;   ///< token���̏���offset�l�B
		typename super_type::stage::event::item::offset package; ///< package���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("set_token_model");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// stage����model-package���擾�B
			psyq::scene_package* const a_package(
				a_stage.get_package(
					a_stage.event_.replace_hash(
						a_parameters->package)).get());
			if (NULL != a_package)
			{
				// stage����token���擾���Amodel��ݒ�B
				typename super_type::stage::token* const a_token(
					a_stage.get_token(
						a_stage.event_.replace_hash(
							a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_model(a_token->scene_, *a_package);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��camera��ݒ肷��event�B
/*
template< typename t_stage >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_section_camera
{
	public: struct parameters
	{
		typename super_type::stage::event::item::offset section;      ///< section���̏���offset�l�B
		typename super_type::stage::event::item::offset camera_token; ///< camera�Ɏg��token���̏���offset�l�B
		typename super_type::stage::event::item::offset camera_node;  ///< camera�Ɏg��node���̏���offset�l�B
		typename super_type::stage::event::item::offset focus_token;  ///< focus�Ɏg��token���̏���offset�l�B
		typename super_type::stage::event::item::offset focus_node;   ///< focus�Ɏg��node���̏���offset�l�B
	}
};
 */

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��light��ݒ肷��event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_section_light:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::set_section_light this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset section; ///< section�̖��O�B
		typename super_type::stage::event::item::offset token;   ///< light�Ƃ��Ďg��token�̖��O�B
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("set_section_light");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// stage����section���擾�B
			typename super_type::stage::section* const a_section(
				a_stage.get_section(
					a_stage.event_.replace_hash(a_parameters->section)).get());
			if (NULL != a_section)
			{
				// stage����light-token���������Asection�ɐݒ�B
				typename super_type::stage::token::shared_ptr const& a_token(
					a_stage.get_token(
						a_stage.event_.replace_hash(a_parameters->token)));
				if (NULL != a_token.get())
				{
					a_section->light_ = a_token;
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief event-line���J�n����event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_event_line:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::set_event_line this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset line;
		typename super_type::stage::event::item::offset points;
		typename super_type::stage::event::item::offset scale;
		typename super_type::stage::event::real         start_frame;
		typename super_type::stage::event::hash::value  start_origin;
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("set_event_line");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// scene-event��event-line��o�^�B
			super_type::stage::event::line* const a_line(
				a_stage.event_.reset_line(
					a_stage.event_.replace_hash(a_parameters->line),
					a_stage.event_.replace_hash(a_parameters->points)));
			if (NULL != a_line)
			{
				// time-scale�̂Ȃ���Ԃ�event-line�ɊJ�nframe��ݒ�B
				a_line->scale_.reset();
				a_line->seek(
					a_parameters->start_frame,
					0 == a_parameters->start_origin? SEEK_SET: SEEK_END);

				// event-line��time-scale��ݒ�B
				a_line->scale_ = a_stage.event_.get_scale(
					a_stage.event_.replace_hash(a_parameters->scale));
				a_line->seek(i_apply.time_, SEEK_CUR);
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief time-scale��ݒ肷��event�B
template<
	typename t_hash,
	typename t_real,
	typename t_string,
	typename t_allocator >
class psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action::set_time_scale:
	public psyq::scene_stage< t_hash, t_real, t_string, t_allocator >::action
{
	typedef typename stage::action super_type;
	typedef typename super_type::set_time_scale this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename super_type::stage::event::item::offset name;  ///< time-scale��������̐擪offset�l�B
		typename super_type::stage::event::item::offset super; ///< ���time-scale��������̐擪offset�l�B
		typename super_type::stage::event::hash::value  frame;
		typename super_type::stage::event::real         start;
		typename super_type::stage::event::real         end;
	};

	//-------------------------------------------------------------------------
	public: static typename super_type::stage::event::hash::value get_hash()
	{
		return super_type::stage::event::hash::generate("set_time_scale");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		typename super_type::stage::event::action::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		typename super_type::stage& a_stage(
			static_cast< typename super_type::apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// scene-event����time-scale���擾�B
			typename super_type::stage::event::line::scale* const a_scale(
				a_stage.event_.get_scale(
					a_stage.event_.replace_hash(a_parameters->name)).get());
			if (NULL != a_scale)
			{
				typename super_type::stage::event::line::scale::lerp const
					a_lerp(
						a_parameters->frame,
#ifdef _WIN32
						::_isnan(a_parameters->start)?
#else
						std::isnan(a_parameters->start)?
#endif // _WIN32
							a_scale->get_scale(): a_parameters->start,
						a_parameters->end);
				typename super_type::stage::event::hash::value const a_super_hash(
					a_stage.event_.replace_hash(a_parameters->super));
				if (super_type::stage::event::hash::EMPTY != a_super_hash)
				{
					// scale�l�Ə��scale��ݒ�B
					a_scale->reset(
						a_lerp, a_stage.event_.get_scale(a_super_hash));
				}
				else
				{
					// scale�l�݂̂�ݒ�B
					a_scale->reset(a_lerp);
				}
			}
		}
	}
};

#endif // !PSYQ_SCENE_EVENT_ACTION_CPP_HPP_
