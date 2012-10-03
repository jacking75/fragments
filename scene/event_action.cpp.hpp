#ifndef PSYQ_SCENE_EVENT_ACTION_CPP_HPP_
#define PSYQ_SCENE_EVENT_ACTION_CPP_HPP_

#include <cmath>
//#include <psyq/scene/scene_stage.hpp>

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-package��p�ӂ���event�B
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::load_package:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::load_package this_type;

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("load_package");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// stage��package��p�ӁB
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		a_stage.get_package(
			a_stage.event_.replace_hash(i_apply.point_.integer));
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��p�ӂ���event�B
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::load_token:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::load_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset token;   ///< �ǉ�����token���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset scale;   ///< �ݒ肷��time-scale���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("load_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// section��token��stage�ɗp�ӁB
			psyq::scene_stage::token* const a_token(
				a_stage.get_token(
					a_stage.event_.replace_hash(a_parameters->token),
					a_stage.event_.replace_hash(a_parameters->section)).get());
			if (NULL != a_token)
			{
				// token��time-scale��ݒ�B
				typename t_hash::value const a_scale(
					a_stage.event_.replace_hash(a_parameters->scale));
				if (t_hash::EMPTY != a_scale)
				{
					a_token->time_scale_ = a_stage.event_.get_scale(a_scale);
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token���폜����event�B
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::unload_token:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::unload_token this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset token;   ///< �폜����token���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("unload_token");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			typename t_hash::value const a_token(
				a_stage.event_.replace_hash(a_parameters->token));
			typename t_hash::value const a_section(
				a_stage.event_.replace_hash(a_parameters->section));
			if (t_hash::EMPTY != a_section)
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
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_token_animation:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::set_token_animation this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset token;
		typename psyq::event_item< t_hash >::offset package;
		typename psyq::event_item< t_hash >::offset flags;
		t_real                                      start;
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_token_animation");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
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
				psyq::scene_stage::token* const a_token(
					a_stage.get_token(
						a_stage.event_.replace_hash(
							a_parameters->token)).get());
				if (NULL != a_token)
				{
					psyq_extern::set_animation(
						a_token->scene_,
						*a_package,
						psyq::scene_stage::event::line::scale::get_scale(
							a_token->time_scale_, i_apply.time_));
				}
			}
		}
	}
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-token��model��ݒ肷��event�B
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_token_model:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::set_token_model this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset token;   ///< token���̏���offset�l�B
		typename psyq::event_item< t_hash >::offset package; ///< package���̏���offset�l�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_token_model");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
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
				psyq::scene_stage::token* const a_token(
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
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_section_camera
{
	typename psyq::event_item< t_hash >::offset section;      ///< section���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset camera_token; ///< camera�Ɏg��token���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset camera_node;  ///< camera�Ɏg��node���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset focus_token;  ///< focus�Ɏg��token���̏���offset�l�B
	typename psyq::event_item< t_hash >::offset focus_node;   ///< focus�Ɏg��node���̏���offset�l�B
};

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/// @brief scene-section��light��ݒ肷��event�B
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_section_light:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::set_section_light this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset section; ///< section�̖��O�B
		typename psyq::event_item< t_hash >::offset token;   ///< light�Ƃ��Ďg��token�̖��O�B
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_section_light");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// stage����section���擾�B
			psyq::scene_stage::section* const a_section(
				a_stage.get_section(
					a_stage.event_.replace_hash(a_parameters->section)).get());
			if (NULL != a_section)
			{
				// stage����light-token���������Asection�ɐݒ�B
				psyq::scene_stage::token::shared_ptr const& a_token(
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
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_event_line:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::set_event_line this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset line;
		typename psyq::event_item< t_hash >::offset points;
		typename psyq::event_item< t_hash >::offset scale;
		t_real                                      start_frame;
		typename t_hash::value                      start_origin;
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_event_line");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// scene-event��event-line��o�^�B
			psyq::scene_stage::event::line* const a_line(
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
template< typename t_hash, typename t_real >
class psyq::event_action< t_hash, t_real >::set_time_scale:
	public psyq::event_action< t_hash, t_real >
{
	typedef typename psyq::event_action< t_hash, t_real > super_type;
	typedef typename super_type::set_time_scale this_type;

	//-------------------------------------------------------------------------
	public: struct parameters
	{
		typename psyq::event_item< t_hash >::offset name;  ///< time-scale��������̐擪offset�l�B
		typename psyq::event_item< t_hash >::offset super; ///< ���time-scale��������̐擪offset�l�B
		typename t_hash::value                      frame;
		t_real                                      start;
		t_real                                      end;
	};

	//-------------------------------------------------------------------------
	public: static typename t_hash::value get_hash()
	{
		return t_hash::generate("set_time_scale");
	}

	//-------------------------------------------------------------------------
	public: virtual void apply(
		super_type::apply_parameters const& i_apply)
	{
		// ���ɂ���������擾�B
		psyq::scene_stage& a_stage(
			static_cast< psyq::scene_stage::event_apply_parameters const& >(
				i_apply).stage_);
		typename this_type::parameters const* const a_parameters(
			a_stage.event_.get_address< typename this_type::parameters >(
				i_apply.point_.integer));
		if (NULL != a_parameters)
		{
			// scene-event����time-scale���擾�B
			psyq::scene_stage::event::line::scale* const a_scale(
				a_stage.event_.get_scale(
					a_stage.event_.replace_hash(a_parameters->name)).get());
			if (NULL != a_scale)
			{
				psyq::scene_stage::event::line::scale::lerp const a_lerp(
					a_parameters->frame,
#ifdef _WIN32
					::_isnan(a_parameters->start)?
#else
					std::isnan(a_parameters->start)?
#endif // _WIN32
						a_scale->get_scale(): a_parameters->start,
					a_parameters->end);
				typename t_hash::value const a_super_hash(
					a_stage.event_.replace_hash(a_parameters->super));
				if (t_hash::EMPTY != a_super_hash)
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
